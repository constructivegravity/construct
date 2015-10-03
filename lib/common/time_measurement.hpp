#pragma once

#include <chrono>
#include <ostream>

namespace Albus {
    namespace Common {

        class TimeMeasurement {
        public:
            TimeMeasurement() {
                start_time = std::chrono::high_resolution_clock::now();
                end_time = start_time;
                stopped = false;
            }
        public:
            void Stop() {
                stopped = true;
                end_time = std::chrono::high_resolution_clock::now();
            }

            bool Stopped() const {
                return stopped;
            }

            friend std::ostream& operator<<(std::ostream& os, const TimeMeasurement& time) {
                if (time.stopped) {
                    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(time.end_time-time.start_time).count();
                    if (millisecs < 1000) {
                        os << millisecs << " ms";
                        return os;
                    }

                    // Calculate seconds
                    auto secs = millisecs / 1000;
                    millisecs = millisecs % 1000;

                    if (secs < 60) {
                        os << secs << "." << millisecs << " s";
                        return os;
                    }

                    auto mins = secs / 60;
                    secs = secs % 60;

                    if (mins < 60) {
                        os << mins << " min " << secs << "." << millisecs << " s";
                        return os;
                    }

                    auto hours = mins / 60;
                    mins = mins % 60;

                    os << hours << " h" << mins << " min" << secs << "." << millisecs << " s";
                } else {
                    os << "Still running ...";
                }
                return os;
            }
        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
            std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
            bool stopped;
        };

    }
}