#ifndef LOGGER_H
#define LOGGER_H

#include "lockqueue.hpp"

#include <string>
#include <iostream>

enum LogLevel
{
    INFO,  // 普通信息
    ERROR, // 错误信息
};

class Logger
{
public:
    // 获取日志的单例
    static Logger &getInstance();
    // 设置log级别
    void setLogLevel(LogLevel level);
    // 写日志
    void log(std::string msg);

private:
    int level_;                        // 记录日志级别
    LockQueue<std::string> lockQueue_; // 日志缓冲队列
    Logger();
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
};

#define LOG_INFO(logmsgformat, ...)                     \
    do                                                  \
    {                                                   \
        Logger &logger = Logger::getInstance();         \
        logger.setLogLevel(INFO);                       \
        char c[1024] = {0};                             \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.log(c);                                  \
    } while (0);

#define LOG_ERROR(logmsgformat, ...)                    \
    do                                                  \
    {                                                   \
        Logger &logger = Logger::getInstance();         \
        logger.setLogLevel(ERROR);                      \
        char c[1024] = {0};                             \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.log(c);                                  \
    } while (0)
;

#endif // LOGGER_H