#pragma once

#include <thread>
#include <mutex>
#include <memory>
#include <queue>
#include <vector>
#include <map>
#include <future>
#include <condition_variable>
#include <functional>
#include <stdexcept>

#include <iostream>

namespace Construction {
    namespace Common {

        class TaskPool {
        public:
            TaskPool(int threads = std::thread::hardware_concurrency()) : terminate(false), stopped(false), remainingTasks(0) {
                threadPool.reserve(threads);

                for (size_t i = 0; i < threads; ++i) {
                    threadPool.emplace_back([this] {
                        while (true) {
                            std::function<void()> task;

                            // Scope based locking
                            {
                                std::unique_lock<std::mutex> lock(this->tasksMutex);

                                // Wait until queue is not empty or termination signal is sent
                                this->condition.wait(lock, [this] {
                                    return this->terminate || !this->tasks.empty();
                                });

                                if (this->terminate && this->tasks.empty()) return;

                                // Move the top task to our reference and remove it from the queue
                                task = std::move(this->tasks.front());
                                this->tasks.pop();
                            }

                            // Execute task
                            task();

                            {
                                std::unique_lock<std::mutex> lock(tasksMutex);

                                // Decrease the number of remaining tasks
                                std::atomic_fetch_sub_explicit(&this->remainingTasks, static_cast<unsigned>(1), std::memory_order_relaxed);
                            }

                            // Notify that a task was finished
                            this->condition_finished.notify_all();
                        }
                    });
                }
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
                    tasks.emplace([task]() { (*task)(); });

                    // Increase the number of active tasks;
                    std::atomic_fetch_add_explicit(&this->remainingTasks, static_cast<unsigned>(1), std::memory_order_relaxed);
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

            // Wait for all tasks to finish
            void Wait() {
                std::unique_lock<std::mutex> lock(tasksMutex);

                this->condition_finished.wait(lock, [this]() { 
                    return this->tasks.empty() && this->remainingTasks == 0; 
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
        private:
            std::vector<std::thread> threadPool;
            std::queue<std::function<void()>> tasks;
            std::atomic<unsigned> remainingTasks;

            mutable std::mutex tasksMutex;
            std::condition_variable condition;
            std::condition_variable condition_finished;

            std::atomic<bool> terminate;
            std::atomic<bool> stopped;
        };

    }

    namespace Parallel {

        template<typename S, typename T>
        inline std::vector<S> Map(std::vector<T> elements, std::function<S(const T&)> fn) {
            Common::TaskPool pool;
            return pool.Map(elements, fn);
        }

    }

}