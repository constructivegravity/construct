#pragma once

#include <thread>
#include <mutex>
#include <memory>
#include <queue>
#include <vector>
#include <future>
#include <condition_variable>
#include <functional>
#include <stdexcept>

namespace Construction {
    namespace Common {

        class TaskPool {
        public:
            TaskPool(int threads) : terminate(false), stopped(false) {
                for (size_t i = 0; i < threads; ++i) {
                    threadPool.emplace_back([this] {
                        for (; ;) {
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
                        }
                    });
                }
            }

            TaskPool() : TaskPool(std::thread::hardware_concurrency()) { }

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
                }

                // Wake up one thread and return the future
                condition.notify_one();
                return res;
            }

            bool Empty() {
                return tasks.empty();
            }

            // Wait for all tasks to finish
            void Wait() {
                std::thread block([&]() {
                    // Scope based locking
                    while (true) {
                        std::unique_lock<std::mutex> lock(tasksMutex);
                        if (tasks.empty()) return;
                    }
                });

                // Wait to finish
                block.join();
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

            std::mutex tasksMutex;
            std::condition_variable condition;

            bool terminate;
            bool stopped;
        };

    }
}