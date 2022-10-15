#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <mutex>
#include <map>
#include <cstdio>
#include <cstring>
#include "config.hpp"

enum logPriority{
    trace       = 5,
    debug       = 4,
    info        = 3,
    warning     = 2,
    error       = 1,
    critical    = 0
};

#define DEBUG_ENABLED 1

#define LOG_TRACE(...)        Logger::Trace(__FILE__,__FUNCTION__, __LINE__, __VA_ARGS__);
#define LOG_INFO(...)         Logger::Info(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

#if DEBUG_ENABLED
    #define LOG_DEBUG(...)    Logger::Debug(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);
#else
    #define LOG_DEBUG(...)
#endif

#define LOG_WARN(...)         Logger::warning(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);
#define LOG_ERR(...)          Logger::Error(__FILE__, __FUNCTION__, __LINE__,  __VA_ARGS__);
#define LOG_CRIT(...)         Logger::Critical(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

#define __FILENAME__(file) (std::strrchr(file, '/') ? std::strrchr(file, '/') + 1 : file)


class Logger
{
    private:
        std::mutex log_mutex;
        logPriority priority = trace;

        template<typename... Args>
        static void log(const char* msg_prio_str, logPriority msg_prio, const char* file,  int line, const char* func, const char* msg, Args... args)
        {
                        
            if(msg_prio <= getInstance().priority){
                {
                    std::unique_lock<std::mutex> lock(getInstance().log_mutex);
                    std::cout << msg_prio_str;
                    std::cout << __FILENAME__(file) << "||" << func << "||" << line << "||";

                    #pragma clang diagnostic push // disabling warnings here
                    #pragma clang diagnostic ignored "-Wformat"
                    #pragma clang diagnostic ignored "-Wformat-nonliteral"
                    std::printf(msg, args...);
                    #pragma clang diagnostic pop

                    std::cout << "\033[0m" << std::endl;
                }
            }
        }

        static Logger& getInstance(){
            static Logger logger;
            return logger;
        }

    public:
        
        static void setPriority(logPriority new_priority)
        {
            getInstance().priority = new_priority;
        }

        template<typename... Args>
        static void Trace(const char* file, const char* func, int line, const char* msg, Args... args)
        {
            log("[Trace]||", trace, file, line, func, msg, args...);
        }

        template<typename... Args>
        static void Debug(const char* file, const char* func, int line, const char* msg, Args... args)
        {
            log("\x1B[33m[Debug]||", debug, file, line, func, msg, args...);
        }

        template<typename... Args>
        static void Info(const char* file, const char* func, int line, const char* msg, Args... args)
        {
            log("\x1B[32m[Info]||", info, file, line, func, msg, args...);
        }

        template<typename... Args>
        static void Warning(const char* file, const char* func, int line, const char* msg, Args... args)
        {
            log("\x1B[93m[Warning]||", warning, file, line, func, msg, args...);
        }

        template<typename... Args>
        static void Error(const char* file, const char* func, int line, const char* msg, Args... args)
        {
            log("\x1B[35m[Error]||", error, file, line, func, msg, args...);
        }

        template<typename... Args>
        static void Critical(const char* file, const char* func, int line, const char* msg, Args... args)
        {
            log("\x1B[31m[Critical]||", critical, file, line, func, msg, args...);
        }
};

#endif
