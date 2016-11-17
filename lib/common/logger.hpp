#pragma once

#include <memory>
#include <unordered_map>
#include <mutex>
#include <iostream>
#include <fstream>

#include <common/datetime.hpp>
#include <common/singleton.hpp>
#include <memory>

namespace Construction {
    namespace Common {

        class AbstractLogger;

        /**
            \class ColorRAII

            RAII helper class to give a color to the output as long as the
            object is in scope and automatically set back to default the
            moment the destructor is called.
         */
        class ColorRAII {
        public:
            enum Color {
                DEFAULT = 39,
                BLACK = 30,
                RED = 31,
                GREEN = 32,
                YELLOW = 33,
                BLUE = 34,
                MAGENTA = 35,
                CYAN = 36,
                LIGHTGRAY = 37,
                DARKGRAY = 90,
                LIGHTRED = 91,
                LIGHTGREEN = 92,
                LIGHTYELLOW = 93,
                LIGHTBLUE = 94,
                LIGHTMAGENTA = 95,
                LIGHTCYAN = 96,
                WHITE = 97
            };
        public:
            ColorRAII(const AbstractLogger* logger, Color color=DEFAULT);
            virtual ~ColorRAII();
        private:
            const AbstractLogger* logger;
            Color color;
        };

        enum class DebugLevel {
            NOTHING = 0,

            SUCCESS = 1,    // always print success messages

            CRITICAL = 2,   // errors nobody can recover from
            ERROR = 3,      //
            WARNING = 4,
            INFO = 5,
            DEBUG = 6
        };

        class AbstractLogger {
        public:
            AbstractLogger(bool isColored, bool includeTimeStamp, DebugLevel level) : coloredOutput(isColored), includeTimeStamp(includeTimeStamp), level(level) { }
            virtual ~AbstractLogger() = default;
        public:
            bool IsColored() const { return coloredOutput; }
            bool IncludesTimeStamp() const { return includeTimeStamp; }

            DebugLevel GetDebugLevel() const { return level; }
            void SetDebugLevel(DebugLevel level) { this->level = level; }
        public:
            virtual void Print(const std::string&) const = 0;
        protected:
            void PrintTimestamp() const {
                if (!includeTimeStamp) return;

                {
                    ColorRAII color (this, ColorRAII::LIGHTGRAY);

                    auto now = Datetime::Now();

                    Print(now.ToString("%F %H:%M:%S  "));
                }
            }
        protected:
            inline bool DebugLevelCheck(const DebugLevel& other) const {
                return static_cast<int>(other) <= static_cast<int>(level);
            }
        public:
            void Nothing(const std::string& content) const {
                // Cancel if not allowed
                if (!DebugLevelCheck(DebugLevel::NOTHING)) return;

                // Lock the mutex
                std::unique_lock<std::mutex> lock(mutex);

                PrintTimestamp();

                Print(content + "\n");
            }

            void Critical(const std::string& content) const {
                // Cancel if not allowed
                if (!DebugLevelCheck(DebugLevel::CRITICAL)) return;

                // Lock the mutex
                std::unique_lock<std::mutex> lock(mutex);

                PrintTimestamp();

                // Make the output red
                {
                    ColorRAII color (this, ColorRAII::RED);
                    Print("Critical: ");
                }

                Print(content + "\n");
            }

            void Error(const std::string& content) const {
                // Cancel if not allowed
                if (!DebugLevelCheck(DebugLevel::ERROR)) return;

                // Lock the mutex
                std::unique_lock<std::mutex> lock(mutex);

                PrintTimestamp();

                // Make the output red
                {
                    ColorRAII color (this, ColorRAII::RED);
                    Print("Error:    ");
                }

                Print(content + "\n");
            }

            void Warning(const std::string& content) const {
                // Cancel if not allowed
                if (!DebugLevelCheck(DebugLevel::WARNING)) return;

                // Lock the mutex
                std::unique_lock<std::mutex> lock(mutex);

                PrintTimestamp();

                // Make the output red
                {
                    ColorRAII color (this, ColorRAII::YELLOW);
                    Print("Warning:  ");
                }

                Print(content + "\n");
            }

            void Success(const std::string& content) const {
                // Cancel if not allowed
                if (!DebugLevelCheck(DebugLevel::SUCCESS)) return;

                // Lock the mutex
                std::unique_lock<std::mutex> lock(mutex);

                PrintTimestamp();

                // Make the output red
                {
                    ColorRAII color (this, ColorRAII::GREEN);
                    Print("Success:  ");
                }

                Print(content + "\n");
            }

            void Info(const std::string& content) const {
                // Cancel if not allowed
                if (!DebugLevelCheck(DebugLevel::INFO)) return;

                // Lock the mutex
                std::unique_lock<std::mutex> lock(mutex);

                PrintTimestamp();

                // Make the output red
                {
                    ColorRAII color (this, ColorRAII::CYAN);
                    Print("Info:     ");
                }

                Print(content + "\n");
            }

            void Debug(const std::string& content) const {
                // Cancel if not allowed
                if (!DebugLevelCheck(DebugLevel::DEBUG)) return;

                // Lock the mutex
                std::unique_lock<std::mutex> lock(mutex);

                PrintTimestamp();

                // Make the output red
                {
                    ColorRAII color (this, ColorRAII::BLUE);
                    Print("Debug:    ");
                }

                Print(content + "\n");
            }
        protected:
            bool coloredOutput=false;
            bool includeTimeStamp;

            mutable std::mutex mutex;
            DebugLevel level;
        };

        class ScreenLogger : public AbstractLogger {
        public:
            ScreenLogger() : AbstractLogger(true, false, DebugLevel::ERROR) { }

            virtual ~ScreenLogger() throw() = default;
        public:
            virtual void Print(const std::string& content) const override {
                std::cout << content;
            }
        };

        class FileLogger : public AbstractLogger {
        public:
            FileLogger(const std::string& filename) : AbstractLogger(false, true, DebugLevel::DEBUG), filename(filename) { }

            virtual ~FileLogger() throw() { }
        public:
            virtual void Print(const std::string& content) const override {
                std::ofstream file (filename, std::fstream::out | std::fstream::app);
                file << content;
                file.close();
            }
        private:
            std::string filename;
        };

        class LoggerManager : public Singleton<LoggerManager> {
        public:
            AbstractLogger* Screen(const std::string& name) {
                AbstractLogger* result = nullptr;

                try {
                    logger[name] = std::unique_ptr<AbstractLogger>(new ScreenLogger());
                    result = logger[name].get();
                } catch (...) {

                }

                return result;
            }

            AbstractLogger* File(const std::string& name, const std::string& filename) {
                AbstractLogger* result = nullptr;

                try {
                    logger[name] = std::unique_ptr<AbstractLogger>(new FileLogger(filename));
                    result = logger[name].get();
                } catch (...) {

                }

                return result;
            }
        public:
            void SetDebugLevel(const std::string& name, DebugLevel level) {
                auto it = logger.find(name);

                if (it == logger.end()) return;

                it->second->SetDebugLevel(level);
            }
        public:
            void Nothing(const std::string& content) const {
                for (auto& pair : logger) {
                    pair.second->Nothing(content);
                }
            }

            void Critical(const std::string& content) const {
                for (auto& pair : logger) {
                    pair.second->Critical(content);
                }
            }

            void Error(const std::string& content) const {
                for (auto& pair : logger) {
                    pair.second->Error(content);
                }
            }

            void Warning(const std::string& content) const {
                for (auto& pair : logger) {
                    pair.second->Warning(content);
                }
            }

            void Success(const std::string& content) const {
                for (auto& pair : logger) {
                    pair.second->Success(content);
                }
            }

            void Info(const std::string& content) const {
                for (auto& pair : logger) {
                    pair.second->Info(content);
                }
            }

            void Debug(const std::string& content) const {
                for (auto& pair : logger) {
                    pair.second->Debug(content);
                }
            }
        private:
            std::unordered_map<std::string, std::unique_ptr<AbstractLogger>> logger;
        };

        // ==---------------------------------------------------------------==
        //                          IMPLEMENTATION
        // ==---------------------------------------------------------------==

        ColorRAII::ColorRAII(const AbstractLogger* logger, Color color) : logger(logger), color(color) {
            if (logger->IsColored()) {
                std::stringstream ss;
                ss << "\033[" << static_cast<int>(color) << "m";

                logger->Print(ss.str());
            }
        }

        ColorRAII::~ColorRAII() {
            if (logger->IsColored()) {
                logger->Print("\033[0m");
            }
        }

    }

    /**
        \class Logger

        \brief Syntactic sugar class for logging

        Syntact sugar class for logging. This implements a streaming logging
        API.

        Registration works via the two static methods `File` and `Screen`. Once
        they are called to generate a logger, these loggers are accessed
        whenever a `Logger` object is created.

        ```
        // Initialization
        auto screenLogger = Construction::Logger::Screen("screen");
        auto screenLogger = Construction::Logger::File("file", "construct.log");

        ...

        // Logging
        Construction::Logger logger;

        logger << Construction::Logger::ERROR << "This is a test" << Construction::Logger::endl;
        ```
     */
    class Logger {
    public:
        Logger() { }
    public:
        static Logger Screen(const std::string& name) {
            Common::LoggerManager::Instance()->Screen(name);
            return Logger();
        }

        static Logger File(const std::string& name, const std::string& filename) {
            Common::LoggerManager::Instance()->File(name, filename);
            return Logger();
        }
    public:
        void SetDebugLevel(const std::string& name, Common::DebugLevel level) const {
            Common::LoggerManager::Instance()->SetDebugLevel(name, level);
        }
    public:
        inline void DoNothing(const std::string& msg) {
            Common::LoggerManager::Instance()->Nothing(msg);
        }

        inline void DoCritical(const std::string& msg) {
            Common::LoggerManager::Instance()->Critical(msg);
        }

        inline void DoError(const std::string& msg) {
            Common::LoggerManager::Instance()->Error(msg);
        }

        inline void DoWarning(const std::string& msg) {
            Common::LoggerManager::Instance()->Warning(msg);
        }

        inline void DoSuccess(const std::string& msg) {
            Common::LoggerManager::Instance()->Success(msg);
        }

        inline void DoInfo(const std::string& msg) {
            Common::LoggerManager::Instance()->Info(msg);
        }

        inline void DoDebug(const std::string& msg) {
            Common::LoggerManager::Instance()->Debug(msg);
        }

        template<typename T>
        std::string Compose(const T& arg) {
            std::stringstream ss;
            ss << arg;
            return ss.str();
        }

        template<typename T, typename... Args>
        std::string Compose(const T& arg, const Args&... args) {
            std::stringstream ss;
            ss << Compose(arg) << Compose(args...);
            return ss.str();
        }

        template<typename... Args>
        static void Nothing(const Args&... args) {
            Logger logger;
            logger.DoNothing(logger.Compose(args...));
        }

        template<typename... Args>
        static void Critical(const Args&... args) {
            Logger logger;
            logger.DoCritical(logger.Compose(args...));
        }

        template<typename... Args>
        static void Error(const Args&... args) {
            Logger logger;
            logger.DoError(logger.Compose(args...));
        }

        template<typename... Args>
        static void Warning(const Args&... args) {
            Logger logger;
            logger.DoWarning(logger.Compose(args...));
        }

        template<typename... Args>
        static void Success(const Args&... args) {
            Logger logger;
            logger.DoSuccess(logger.Compose(args...));
        }

        template<typename... Args>
        static void Info(const Args&... args) {
            Logger logger;
            logger.DoInfo(logger.Compose(args...));
        }

        template<typename... Args>
        static void Debug(const Args&... args) {
            Logger logger;
            logger.DoDebug(logger.Compose(args...));
        }

    /** ==------------------- STREAMING ----------------------------== */
    public:
        enum Flag {
            NOTHING,
            CRITICAL,
            ERROR,
            WARNING,
            INFO,
            SUCCESS,
            DEBUG,
            endl
        };

        struct Line {
            Flag flag;
            std::string content;
        };

        template<typename T>
        friend Logger& operator<<(Logger& logger, const T& content) {
	    {
		std::stringstream ss;
		ss << logger.currentLine.content << content;
		logger.currentLine.content = ss.str();
	    }
            return logger;
        }

        friend Logger& operator<<(Logger& logger, const Flag& flag) {
            if (flag == endl) {
                std::string msg = logger.currentLine.content;

                // Assemble the message
                switch (logger.currentLine.flag) {
                    case NOTHING:
                        logger.DoNothing(msg);
                        break;

                    case CRITICAL:
                        logger.DoCritical(msg);
                        break;

                    case ERROR:
                        logger.DoError(msg);
                        break;

                    case INFO:
                        logger.DoInfo(msg);
                        break;

                    case WARNING:
                        logger.DoWarning(msg);
                        break;

                    case SUCCESS:
                        logger.DoSuccess(msg);
                        break;

                    case DEBUG:
                        logger.DoDebug(msg);
                        break;

                    case endl:
                        break;
                };

                // Clean the line
                logger.currentLine = Line();
                return logger;
            }

            logger.currentLine.flag = flag;
            return logger;
        }
    private:
        Line currentLine;
    };

}
