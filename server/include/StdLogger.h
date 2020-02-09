#pragma once
//
// Created by dmitrii on 2/9/20.
//

#ifndef TIME_INTERVAL_SUM_SERVER_STDLOGGER_H
#define TIME_INTERVAL_SUM_SERVER_STDLOGGER_H

#include <iostream>
#include <memory>
#include <thread>
#include <sstream>
#include <unistd.h>

#include "ILogger.h"

class StdLogger : public ILogger
{
private:
    struct MsgCategory
    {
        static constexpr const char* info() { return  "INFO"; }
        static constexpr const char* warn() { return  "WARN"; }
        static constexpr const char* error() { return  "ERROR"; }
    };

    struct Color
    {
        static constexpr const char* red()     { return  "\033[0;31m"; }
        static constexpr const char* green()   { return  "\033[1;32m"; }
        static constexpr const char* yellow()  { return  "\033[1;33m"; }
        static constexpr const char* cyan()    { return  "\033[0;36m"; }
        static constexpr const char* magenta() { return  "\033[0;35m"; }
        static constexpr const char* reset()   { return  "\033[0m";    }
    };

public:
    void raw(const std::string& msg) override
    {
        std::cout << msg;
    }

    void info(const std::string& msg) override
    {
        print(std::cout, MsgCategory::info(), Color::cyan(), msg);
    }

    void warn(const std::string& msg) override
    {
        print(std::cout, MsgCategory::warn(), Color::yellow(), msg);
    }

    void error(const std::string& msg) override
    {
        print(std::cerr, MsgCategory::error(), Color::red(), msg);
    }

private:
    void print(std::ostream& out, const char* category, const char* color, const std::string& msg)
    {
        std::stringstream fullMsg;
        fullMsg << color << "[" << category << "][PID: "
               << getpid() << "][THREAD ID: "
               << std::this_thread::get_id() << "]:  "
               << msg
               << Color::reset();
        out << fullMsg.str();
    }

};

#endif //TIME_INTERVAL_SUM_SERVER_STDLOGGER_H
