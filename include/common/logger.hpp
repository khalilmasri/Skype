#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <mutex>
#include <map>
#include <cstdio>
#include <cstring>

enum logPriority{
    trace       = 0, 
    debug       = 1, 
    info        = 2, 
    warning     = 3, 
    error       = 4,
    critical    = 5
};

#define DEBUG_ENABLED 1

#define LOG_TRACE(format...)        Logger::Trace(__FILE__,__FUNCTION__, __LINE__, format);
#define LOG_INFO(format...)         Logger::Info(__FILE__, __FUNCTION__, __LINE__, format);

#if DEBUG_ENABLED
    #define LOG_DEBUG(format...)    Logger::Debug(__FILE__, __FUNCTION__, __LINE__, format);
#else
    #define LOG_DEBUG(format...)
#endif

#define LOG_WARN(format...)         Logger::warning(__FILE__, __FUNCTION__, __LINE__, format);
#define LOG_ERR(format...)          Logger::Error(__FILE__, __FUNCTION__, __LINE__,  format);
#define LOG_CRIT(format...)         Logger::Critical(__FILE__, __FUNCTION__, __LINE__, format);

#define __FILENAME__(file) (std::strrchr(file, '/') ? std::strrchr(file, '/') + 1 : file)


class Logger
{
    private:
        std::mutex log_mutex;
        logPriority priority = critical;

        template<typename... Args>
        static void log(const char* msg_prio_str, logPriority msg_prio, const char* file,  int line, const char* func, const char* msg, Args... args)
        {
                        
            if(msg_prio <= getInstance().priority){
                {
                    std::unique_lock<std::mutex> lock(getInstance().log_mutex);
                    std::cout << msg_prio_str;
                    std::cout << __FILENAME__(file) << "||" << func << "||" << line << "||";
                    std::printf(msg, args...);
                    std::cout << std::endl;
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
            log("[Debug]||", debug, file, line, func, msg, args...);
        }

        template<typename... Args>
        static void Info(const char* file, const char* func, int line, const char* msg, Args... args)
        {
            log("[Info]||", info, file, line, func, msg, args...);
        }

        template<typename... Args>
        static void Warning(const char* file, const char* func, int line, const char* msg, Args... args)
        {
            log("[Warning]||", warning, file, line, func, msg, args...);
        }

        template<typename... Args>
        static void Error(const char* file, const char* func, int line, const char* msg, Args... args)
        {
            log("[Error]||", error, file, line, func, msg, args...);
        }

        template<typename... Args>
        static void Critical(const char* file, const char* func, int line, const char* msg, Args... args)
        {
            log("[Critical]||", critical, file, line, func, msg, args...);
        }
};

#endif
