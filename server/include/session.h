#pragma once
//
// Created by dkuch on 07.02.2020.
//

#ifndef TIME_INTERVAL_SUM_SERVER_SESSION_H
#define TIME_INTERVAL_SUM_SERVER_SESSION_H


#include "server/asio/service.h"
#include "server/asio/tcp_server.h"
#include "system/cpu.h"

#include "calculator.h"
#include "parser.h"

using namespace CppCommon;
using namespace CppServer::Asio;

// It is not really fast, but com'n it's MUCH faster then std::to_string
std::string fast_to_string(int64_t val)
{
    constexpr size_t kMaxNumberSize = 35;
    char result[kMaxNumberSize];

    size_t index = 0;
    bool neg = false;
    if (val < 0)
    {
        neg = true;
        val = -val;
    }

    do
    {
        result[kMaxNumberSize - 1 - index++] = (val % 10) + 48;
        val /= 10;
    }
    while(val);

    if (neg)
        result[kMaxNumberSize - 1 - index] = '-';

    return {result + kMaxNumberSize - neg - index, index + neg };
}

struct ClientAddress
{
    std::string ip;
    std::string port;
};

template <typename InputNumberType, typename TimeProvider = StdTimeProvider>
class TimeIntervalSumSession : public TCPSession
{
public:
    TimeIntervalSumSession(const std::shared_ptr<TCPServer>& server, size_t sumTimeIntervalInMilliseconds) :
            TCPSession(server)
            , m_sumCalculator(sumTimeIntervalInMilliseconds)
    {}

protected:
    void onConnected() override
    {
        m_client =
        {
             socket().remote_endpoint().address().to_string()
            ,std::to_string(socket().remote_endpoint().port())
        };
        m_sessionStartTime = m_timeProvider.Now();
    }

    void onReceived(const void* buffer, size_t size) override
    {
        auto anyData = m_parser.Parse(buffer, size);
        if (anyData)
        {
            handle(*anyData);
            while(auto val = m_parser.Next())
            {
                handle(*val);
            }
        }
    }

private:
    void handle(int64_t value)
    {
        m_sumCalculator.put(value);
        auto curSumStr = fast_to_string(m_sumCalculator.get()) + "\n";

        if (!SendAsync(curSumStr.data(), curSumStr.size()))
        {
            throw std::runtime_error("Can not send data. Unknown reason. Terminate server.");
        }

        auto secondsSinceStart = fast_to_string((m_timeProvider.Now() - m_sessionStartTime) / 1000);
        auto msg = secondsSinceStart + ". Got " + fast_to_string(value) + " from " +
                   m_client.ip + ":" + m_client.port + "(session ID: " + KSessionId + "). "
                   + "The current sum is " + curSumStr;
        std::cout << msg;
    }

protected:
    ClientAddress m_client;
    const std::string KSessionId = id().string();
    const TimeProvider m_timeProvider = TimeProvider{};
    size_t m_sessionStartTime = -1;
    TimeIntervalSumCalculator<InputNumberType, TimeProvider> m_sumCalculator;
    NumbersStreamParser<InputNumberType> m_parser{'\r', 32};
};

#endif //TIME_INTERVAL_SUM_SERVER_SESSION_H
