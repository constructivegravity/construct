#pragma once

#include <chrono>
#include <ostream>
#include <stack>
#include <string>
#include <iostream>

#include <common/singleton.hpp>

namespace Construction {
    namespace Common {

        class TimeMeasurement {
        public:
            TimeMeasurement() {
                Start();
            }
        public:
            void Start() {
                start_time = std::chrono::high_resolution_clock::now();
                end_time = start_time;
                stopped = false;
            }

            void Stop() {
                stopped = true;
                end_time = std::chrono::high_resolution_clock::now();
            }

            bool Stopped() const {
                return stopped;
            }

            friend std::ostream& operator<<(std::ostream& os, const TimeMeasurement& time) {
                auto end_time = (time.stopped) ? time.end_time : std::chrono::high_resolution_clock::now();

                auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(end_time-time.start_time).count();
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

                return os;
            }
        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
            std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
            bool stopped;
        };

        class TimeMeasurementManager : public Singleton<TimeMeasurementManager> {
        public:
            void Start(const std::string& name) {
                measurements.push({ name, { measurements.size() , TimeMeasurement() } });
            }

            void Stop() {
                // Stop measurement
                measurements.top().second.second.Stop();

                std::cout << "\033[90m";
                for (int i=0; i<measurements.top().second.first; i++) {
                   std::cout << "   "; 
                } 
                std::cout << measurements.top().first << " : " << measurements.top().second.second << "\033[0m" << std::endl;

                // Remove the element
                measurements.pop();
            }
        private:
            std::stack< std::pair<std::string, std::pair<unsigned, TimeMeasurement> > > measurements;
        };

    }

    static Common::TimeMeasurementManager* Profiler() {
        return Common::TimeMeasurementManager::Instance();
    }

}