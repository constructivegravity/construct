#pragma once

#include <iostream>
#include <iomanip>

namespace Construction {
    namespace Common {

        class ProgressBar {
        public:
            ProgressBar(unsigned max, unsigned width) : width(width), max(max) {
                started = false;
                pos = 0;

                std::thread thread ([this]() {
                    while (true) {
                        this->Print();
                        std::this_thread::sleep_for(
                                std::chrono::milliseconds(500)
                        );
                    }
                });

                thread.detach();
            }
        public:
            unsigned GetWidth() const { return width; }
            unsigned GetPosition() const { return pos; }
        public:
            void Start() {
                std::unique_lock<std::mutex> lock(mutex);

                started = true;
            }

            void Increase() {
                std::unique_lock<std::mutex> lock(mutex);

                if (pos < max) pos++;
            }

            inline ProgressBar& operator++() {
                Increase();
                return *this;
            }

            inline ProgressBar& operator++(int) {
                Increase();
                return *this;
            }

            void Print() {
                std::unique_lock<std::mutex> lock(mutex);

                if (!started) return;

                unsigned p = static_cast<unsigned>(static_cast<double>(pos)/max * width);

                std::cerr << "  [";
                for (unsigned i=0; i<=p; i++) {
                    std::cerr << "=";
                }
                for (unsigned i=p+1; i<= width; i++) {
                    std::cerr << " ";
                }
                std::cerr << "] ";

                std::cerr << std::fixed << std::setprecision(1) << (static_cast<double>(pos)/max * 100) << " % ";

                std::cerr << "(" << pos << " / " << max << ")";

                // Go back to the beginning
                std::cerr << "\r";
            }
        private:
            unsigned pos;
            unsigned max;
            unsigned width;

            bool started;
            std::mutex mutex;
        };

    }
}