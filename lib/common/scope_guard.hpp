#pragma once

#include <vector>
#include <functional>

namespace Construction {
    namespace Common {

        /**
            \class ScopeGuard

            \brief Scope guard to rollback in case of an error

            Scope guard implementation that allows to rollback changes
            if an exception is thrown somewhere in a function. This allows
            to implement the ACID principle in the code.

            Example:

                database.Insert(element);
                ScopeGuard guard(&Database::Remove, database, element);

                // If now an exception is thrown, the guard will
                // clean up
                // ...

                // If everything is fine to this point, dismiss the guard
                guard.Dismiss();
         */
        class ScopeGuard {
        public:
            typedef std::function<void()>   RollbackFunction;
        public:
            ScopeGuard(RollbackFunction rollback) {
                Guard(rollback);
            }

            template<typename Arg, typename... Args>
            ScopeGuard(std::function<void(const Arg& arg, const Args&... args)> rollback) {
                Guard(rollback);
            }
        public:
            /**
                \brief Rollback if the guard was not dismissed

                When the scope guard goes out of scope, the destructor is called.
                If the guard was not dismissed, it will call all the rollbacks
                in the order they were added.

                If one of the rollback functions can throw an exception, it is
                ignored and the remaining functions are called, since there is
                nothing we can do about this anyway. In practice one should
                only use rollback functions that have a no-exception garantuee.
             */
            ~ScopeGuard() {
                // If the scope guard was not dismissed
                // execute rollbacks
                if (!dismissed) {
                    // Iterate over all rollback functions
                    for (auto& fn : rollbacks) {
                        // Execute rollback
                        try {
                            fn();
                        } catch(...) {
                            // Ignore errors in rollback, since there is nothing
                            // we can do
                        }
                    }
                }
            }
        public:
            /**
                \brief Dismiss the scope guard

                Dismiss the scope guard. This means, that no rollback will occur
             */
            void Dismiss() {
                dismissed = true;
            }
        public:
            /**
                \brief Guard another part

                Adds another rollback function to the guard, that will be executed
                in case of an error.
             */
            void Guard(RollbackFunction rollback) {
                rollbacks.push_back(rollback);
            }

            /**
                \brief Guard another part

                Adds another rollback function to the guard, that will be executed
                in case of an error.
             */
            template<typename Arg, typename... Args>
            void Guard(std::function<void(const Arg&, const Args&...)> rollback, const Arg& arg, const Args&... args) {
                rollbacks.push_back(std::bind(rollback, arg, args...));
            }
        private:
            bool dismissed=false;
            std::vector<RollbackFunction> rollbacks;
        };

    }
}