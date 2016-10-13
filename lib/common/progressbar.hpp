#pragma once

#include <iostream>
#include <iomanip>

namespace Construction {
    namespace Common {

        class ProgressBar {
        public:
            ProgressBar(unsigned max, unsigned width) : width(width), max(max) {
                started = false;
                running = true;
                pos = 0;

                thread = std::thread([this]() {
                    while (true) {
                        this->Print();
                        std::this_thread::sleep_for(
                                std::chrono::milliseconds(500)
                        );

                        if (!running) break;
                    }
                });
            }

            ~ProgressBar() {
                thread.join();
            }
        public:
            unsigned GetWidth() const { return width; }
            unsigned GetPosition() const { return pos; }
        public:
            void Start() {
                started = true;
                time.Start();
            }

            void Increase() {
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

            void Clean() {
                for (int i=0; i<width + 100; i++) {
                    std::cerr << " ";
                }
                std::cerr << "\r";
            }

            void Print() {
                if (pos >= max || !running) {
                    running = false;
                    return;
                }

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

                std::cerr << "(" << pos << " / " << max << ")  " << time << "    ";

                // Go back to the beginning
                std::cerr << "\r";
            }
        private:
            std::atomic<unsigned> pos;
            std::atomic<unsigned> max;
            std::atomic<unsigned> width;

            std::thread thread;
            TimeMeasurement time;

            bool started;
            bool running;
        };

    }
}
