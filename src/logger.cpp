#include "logger.h"

#include <ctime>

Logger::Logger()
{
    // 启动专门的写日志线程
    std::thread writeLogTask([&]()
                             {
                                 while (true)
                                 {
                                     std::time_t now = time(nullptr);
                                     tm *nowTm = std::localtime(&now);

                                     char fileName[128] = "";
                                     sprintf(fileName, "%d-%d-%d-log.txt", nowTm->tm_year + 1900, nowTm->tm_mon + 1, nowTm->tm_mday);

                                     FILE *pf = fopen(fileName, "a+");
                                     if (pf == nullptr)
                                     {
                                         std::cout << "logger file : " << fileName << " open error " << std::endl;
                                         exit(EXIT_FAILURE);
                                     }
                                     std::string msg = lockQueue_.pop();
                                     char timeBuf[128] = {0};
                                     sprintf(timeBuf, "%d:%d:%d => [%s] ", nowTm->tm_hour, nowTm->tm_min, nowTm->tm_sec, (level_ == INFO ? "info" : "error"));
                                     msg.insert(0, timeBuf);
                                     msg.append("\n");
                                     fputs(msg.c_str(), pf);
                                     fclose(pf);
                                 } });
    writeLogTask.detach();
}

Logger &Logger::getInstance()
{
    // TODO: 在此处插入 return 语句
    static Logger logger;
    return logger;
}

void Logger::setLogLevel(LogLevel level)
{
    level_ = level;
}

void Logger::log(std::string msg)
{
    lockQueue_.push(msg);
}
