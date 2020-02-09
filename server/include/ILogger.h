#pragma once
//
// Created by dmitrii on 2/9/20.
//

#ifndef TIME_INTERVAL_SUM_SERVER_ILOGGER_H
#define TIME_INTERVAL_SUM_SERVER_ILOGGER_H

class ILogger
{
public:
    virtual ~ILogger() {}

    // Prints message "as is"
    virtual void raw(const std::string& msg) = 0;

    // Prints message as information
    virtual void info(const std::string& msg) = 0;

    // Prints message as warning
    virtual void warn(const std::string& msg) = 0;

    // Prints message as error
    virtual void error(const std::string& msg) = 0;
};
using ILoggerPtr = std::shared_ptr<ILogger>;


#endif //TIME_INTERVAL_SUM_SERVER_ILOGGER_H
