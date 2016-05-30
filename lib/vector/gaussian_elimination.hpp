#pragma once

#include <vector>
#include

namespace Construction {
    namespace Vector {

        class GaussianEliminationMessage {
        public:
            enum Type {
                END = 1,
                ROW = 2
            };
        public:
            GaussianEliminationMessage(Type type) : type(type) { }
        public:
            bool IsEnd() const { return type == END; }
            bool IsRow() const { return type == ROW; }
        private:
            Type type;
        };

        class GaussianEliminationRowMessage : public GaussianEliminationMessage {
        public:
            GaussianEliminationRowMessage() : GaussianEliminationMessage(ROW) { }
        public:
            Vector row;
        };

        class GaussianEliminationWorker {
        public:
            enum State {
                RUNNING = 1,
                ERROR = 2,
                DONE = 3
            };
        public:
            bool IsRunning() const {
                std::unique_lock<std::mutex> lock(stateMutex);
                return state == RUNNING;
            }

            bool IsError() const {
                std::unique_lock<std::mutex> lock(stateMutex);
                return state == ERROR;
            }

            bool IsFinished() const {
                std::unique_lock<std::mutex> lock(stateMutex);
                return state == RUNNING;
            }
        private:
            State state;
            std::mutex stateMutex;
        };

    }
}