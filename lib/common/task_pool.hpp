#pragma once

#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>
#include <memory>
#include <queue>
#include <stack>
#include <vector>
#include <map>
#include <future>
#include <condition_variable>
#include <functional>
#include <stdexcept>

#include <common/singleton.hpp>
#include <common/logger.hpp>

namespace Construction {
    namespace Common {

        class TaskPool {
        public:
            TaskPool(int threads = std::thread::hardware_concurrency()) : terminate(false), stopped(false) {
                threadPool.reserve(threads);

                for (size_t i = 0; i < threads; ++i) {
                    threadPool.emplace_back([this] {
                        while (true) {
                            std::function<void()> task;

                            std::thread::id id;

                            // Scope based locking
                            {
                                std::unique_lock<std::mutex> lock(this->tasksMutex);

                                // Wait until queue is not empty or termination signal is sent
                                this->condition.wait(lock, [this] {
                                    return this->terminate || !this->tasks.empty();
                                });

                                if (this->terminate && this->tasks.empty()) return;

                                id = this->tasks.front().first;

                                // Move the top task to our reference and remove it from the queue
                                task = std::move(this->tasks.front().second);
                                this->tasks.erase(this->tasks.begin());
                            }


                            // Execute task
                            task();

                            {
                                std::unique_lock<std::mutex> lock(tasksMutex);

                                // Decrease the number of remaining tasks
                                std::atomic_fetch_sub_explicit(this->remainingTasks[id].get(), static_cast<unsigned>(1), std::memory_order_relaxed);
			    }

                            // Notify that a task was finished
                            this->condition_finished.notify_all();
                        }
                    });
                }

                // Start the observer
                /*observer = std::thread([&]() {
                    while (true) {
                        // First check if there is recursion in the helpers
                        std::thread::id current_helper;
                        if (HasRecursionInHelper(&current_helper)) {
                            // If the worker already has an helper, wait for
                            // it to be finished
                            if (helper_ids[current_helper] > 3) {
                                continue;
                            }

                            // spawn a helper for the top helper
                            SpawnHelper(current_helper, false); 

                            // Add the ID to the list
                            helper_ids[current_helper] += 1;
                            continue;
                        }

                        // No recursion in any helpers, check if a worker is stuck
                        std::thread::id current;
                        if (HasRecursion(&current)) {
                            // If the worker already has an helper, wait for
                            // it to be finished
                            if (worker_ids[current] > 3) {
                                continue;
                            }

                            // Spawn a helper for this
                            SpawnHelper(current, true); 

                            // Add the ID to the list
                            worker_ids[current] += 1;
                            continue;
                        }

                        // Check if the shut down signal was sent
                        if (this->terminate && this->tasks.empty()) return;
                    }
                });*/
            }

            TaskPool(const TaskPool&) = delete;
            TaskPool& operator=(const TaskPool&) = delete;
            TaskPool(TaskPool&&) = delete;
            TaskPool& operator=(TaskPool&&) = delete;

            ~TaskPool() {
                if (!stopped) {
                    Shutdown();
                }
            }

        public:
            template<class F, class... Args>
            auto EnqueueCopy(F &&f, Args ... args)
            -> std::future<typename std::result_of<F(Args...)>::type> {
                return Enqueue(f, args...);
            }

            /**
                Enqueue some task to the thread pool. It may obtain parameters.

                @param F		The function to execute
                @param args		The arguments for the function
             */
            template<class F, class... Args>
            auto Enqueue(F &&f, Args &&... args)
            -> std::future<typename std::result_of<F(Args...)>::type> {
                using return_type = typename std::result_of<F(Args...)>::type;

                // Make a shared_ptr to the task, where the arguments are already forwarded, and the result
                // is given by a std::future
                auto task = std::make_shared<std::packaged_task<return_type()> >(
                        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                );

                // Obtain future to the result
                std::future<return_type> res = task->get_future();

                // Scope based locking
                {
                    std::unique_lock<std::mutex> lock(tasksMutex);

                    // don't allow enqueueing after stopping the pool
                    if (terminate || stopped)
                        throw std::runtime_error("enqueue on stopped TaskPool");

                    // Put task in the queue
                    std::pair< std::thread::id, std::function<void()> > obj = { std::this_thread::get_id() , [task]() {
                        (*task)();
                    } };

                    tasks.emplace_back(obj);

                    // If the thread has no tasks so far, add to the list
                    if (this->remainingTasks.find(std::this_thread::get_id()) == this->remainingTasks.end()) {
			this->remainingTasks.insert({ std::this_thread::get_id(), std::make_shared<std::atomic<unsigned>>(0) });
                    }

                    // Increase the number of active tasks;
                    std::atomic_fetch_add_explicit(this->remainingTasks[std::this_thread::get_id()].get(), static_cast<unsigned>(1), std::memory_order_relaxed);
 
                }

                // Wake up one thread and return the future
                condition.notify_one();
                
                return res;
            }

            bool Empty() {
                return tasks.empty();
            }

            template<typename S, typename T>
            std::vector<S> Map(std::vector<T> elements, std::function<S(const T&)> fn) {
                std::map<unsigned, S> results;
                std::mutex resultsMutex;

                // Enqueue all elements
                for (int i=0; i<elements.size(); i++) {
                    Enqueue([&results, &fn, &resultsMutex](unsigned id, const T& value) {
                        // Calculate the element
                        S e = fn(value);

                        // Lock the mutex
                        std::unique_lock<std::mutex> lock(resultsMutex);

                        results.insert({ id, std::move(e) });
                    }, i, elements[i]);
                }

                // Wait for all tasks to finish
                Wait();

                std::vector<S> result;
                for (auto& pair : results) {
                    result.push_back(std::move(pair.second));
                }

                return result;
            }

            template<typename S, typename T>
            std::vector<S> MapEmit(std::vector<T> elements, std::function<void(const T&, std::function<void(S&&)>)> fn) {
                std::map<unsigned, S> results;
                std::mutex resultsMutex;

                // Enqueue all elements
                for (int i=0; i<elements.size(); i++) {
                    Enqueue([&results, &fn, &resultsMutex](unsigned id, const T& value) {
                        // Calculate the element
                        fn(value, [&](S&& value) {
                            // Lock the mutex
                            std::unique_lock<std::mutex> lock(resultsMutex);

                            results.insert({ id, std::move(value) });
                        });
                    }, i, elements[i]);
                }

                // Wait for all tasks to finish
                Wait();

                std::vector<S> result;
                for (auto& pair : results) {
                    result.push_back(std::move(pair.second));
                }

                return result;
            }

            // Wait for all tasks to finish
            void Wait() {
                std::unique_lock<std::mutex> lock(tasksMutex);

                this->condition_finished.wait(lock, [this]() -> bool {
		            auto it = this->remainingTasks.find(std::this_thread::get_id());
		            if (it == this->remainingTasks.end()) return true;
                    return *(it->second) == 0;
                });
            }

            void Shutdown() {
                // Scope based locking
                {
                    // Put unique lock on task mutex.
                    std::unique_lock<std::mutex> lock(tasksMutex);

                    // Set termination flag to true.
                    terminate = true;
                }

                // Wake up all threads.
                condition.notify_all();

                // Join all threads.
                for (auto &thread : threadPool) {
                    thread.join();
                }

                // Empty workers vector.
                threadPool.empty();

                // Indicate that the pool has been shut down.
                stopped = true;
            }
        protected:
            bool HasRecursion(std::thread::id id) const {
                for (auto& pair : tasks) {
                    if (pair.first == id) return true;
                }
                return false;
            }

            /**
                \brief Checks if one of the workers added a task

                Checks if one of the workers added a task. This is necessary
                to detect recursive job spawning that potentielly freezes the
                task pool.

                \param out      The ID to the first worker that has a pending task
             */
            bool HasRecursion(std::thread::id* out = nullptr) const {
                for (auto& thread : threadPool) {
                    auto id = thread.get_id();

                    if (HasRecursion(id)) {
                        if (out) (*out) = id;
                        return true;
                    }
                }

                return false;
            }

            /**
                \brief Check if there is recursion in a helper
             */
            bool HasRecursionInHelper(std::thread::id* out = nullptr) const {
                if (helpers.empty()) return false;

                for (auto& helper : helpers) {
                    if (HasRecursion(helper.get_id())) {
                        if (out) (*out) = helper.get_id();
                        return true;
                    }
                }

                return false;
            }

            /**
                \brief Spawn a helper thread to get rid of the recursion of the given worker

                Spawn a helper thread to get rid of the recursion of the given worker.

                \param id       The id of the worker
             */
            void SpawnHelper(std::thread::id id, bool fromWorker=true) {
                helpers.emplace_back(std::bind([&](std::thread::id id, bool fromWorker) {
                    while (true) {
                        std::function<void()> task;

                        Construction::Logger::Debug("Began helper loop in thread ", std::this_thread::get_id(), " for thread ", id);

                        // Choose task where the id is tasks
                        {
                            std::unique_lock<std::mutex> lock (tasksMutex);

                            bool found=false;

                            for (auto it = tasks.begin(); it != tasks.end(); ++it) {
                                if (it->first == id) {
                                    task = std::move(it->second);
                                    found = true;

                                    // Delete the task from the queue
                                    tasks.erase(it);
                                    break;
                                }
                            }

                            // No more recursion, kill the helper
                            if (!found) {
                                return;
                            }
                        }

                        Construction::Logger::Debug("Started working on task from thread ", id);

                        // Execute the task
                        task();

                        Construction::Logger::Debug("Finished task from thread ", id);

                        // Execute the task in the helper thread
                        {
                            std::unique_lock<std::mutex> lock (tasksMutex);

                            // Decrease the number of remaining tasks
			    std::atomic_fetch_sub_explicit(this->remainingTasks[id].get(), static_cast<unsigned>(1), std::memory_order_relaxed);

                            if (fromWorker) {
                                worker_ids[id]--;
                            } else helper_ids[id]--;
                        }

                        // Notify all the workers for this task to be finished
                        this->condition_finished.notify_all();
                    }
                }, id, fromWorker));

                auto id_ = helpers[helpers.size()-1].get_id();
                Construction::Logger::Debug("New helper: ", id_, " (Number of helpers: ", helpers.size(), ")");
            }
        private:
            std::vector<std::thread> threadPool;
            std::vector<std::thread> helpers;
            std::vector< std::pair<std::thread::id, std::function<void()>> > tasks;
            std::map<std::thread::id, std::shared_ptr<std::atomic<unsigned>>> remainingTasks;
            std::map<std::thread::id, unsigned> worker_ids;
            std::map<std::thread::id, unsigned> helper_ids;
            std::thread observer;

            mutable std::mutex tasksMutex;
            std::condition_variable condition;
            std::condition_variable condition_finished;

            std::atomic<bool> terminate;
            std::atomic<bool> stopped;
        };

    }

    namespace Parallel {

        class GlobalTaskPool : public Singleton<GlobalTaskPool> {
        public:
            // Initialize the global task pool
            GlobalTaskPool() : pool(std::thread::hardware_concurrency()) { }
        public:
            template<class F, class... Args>
            auto Enqueue(F &&f, Args &&... args)
            -> std::future<typename std::result_of<F(Args...)>::type> {
                return pool.Enqueue(f, args...);
            }

            template<typename S, typename T>
            std::vector<S> Map(std::vector<T> elements, std::function<S(const T&)> fn) {
                return pool.Map(elements, fn);
            }

            template<typename S, typename T>
            std::vector<S> MapEmit(std::vector<T> elements, std::function<void(const T&, std::function<void(S&&)>)> fn) {
                return pool.MapEmit(elements, fn);
            }

            void Wait() {
                pool.Wait();
            }
        private:
            Common::TaskPool pool;
        };

        template<typename S, typename T>
        inline std::vector<S> Map(std::vector<T> elements, std::function<S(const T&)> fn) {
            return GlobalTaskPool::Instance()->Map(elements, fn);
        }

        template<typename S, typename T>
        inline std::vector<S> MapEmit(std::vector<T> elements, std::function<void(const T&, std::function<void(S&&)>)> fn) {
            return GlobalTaskPool::Instance()->MapEmit(elements, fn);
        }

    }

}
