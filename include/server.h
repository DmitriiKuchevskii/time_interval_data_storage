#pragma once
//
// Created by dkuch on 07.02.2020.
//
#ifndef TIME_INTERVAL_SUM_SERVER_SERVER_H
#define TIME_INTERVAL_SUM_SERVER_SERVER_H

#include "session.h



template <typename InputNumberType, typename TimeProvider = StdTimeProviderr>
class TimeIntervalSumServer : public TCPServer
{
public:
    TimeIntervalSumServer(size_t sumTimeIntervalInMilliseconds,
                          , uint32_t port = 1111
                          , std::shared_ptr<Service> service = std::make_shared<Service>(std::thread::hardware_concurrency(), true)) :
        TCPServer(port, service)
        , kSumTimeIntervalInMilliseconds(sumTimeIntervalInMilliseconds)
    {
        SetupReuseAddress(true);
        SetupReusePort(true);
        SetupNoDelay(true);
    }

protected:
    std::shared_ptr<TCPSession> CreateSession(const std::shared_ptr<TCPServer>& server) override
    {
        return std::make_shared<SumServerSession<InputNumberType, TimeProvider>>(
                server, kSumTimeIntervalInMilliseconds
        );
    }

private:
    const size_t kSumTimeIntervalInMilliseconds;
};

#endif //TIME_INTERVAL_SUM_SERVER_SERVER_H
