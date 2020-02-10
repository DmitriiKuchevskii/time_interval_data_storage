#pragma once
//
// Created by dkuch on 07.02.2020.
//

#ifndef TIME_INTERVAL_SUM_SERVER_TIMEINTERVALSUMSESSION_H
#define TIME_INTERVAL_SUM_SERVER_TIMEINTERVALSUMSESSION_H


#include "server/asio/service.h"
#include "server/asio/tcp_server.h"
#include "system/cpu.h"

#include "TimeIntervalSumCalculator.h"
#include "BacklogNumbersStreamParser.h"
#include "StdLogger.h"
#include "StdTimeProvider.h"

using namespace CppCommon;
using namespace CppServer::Asio;

class TimeIntervalSumSessionException : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};

// It is not really fast, but com'n it's MUCH faster then std::to_string
template <typename NumberType>
inline std::string fast_to_string(const NumberType& number)
{
    if constexpr (std::numeric_limits<NumberType>::is_integer)
    {
        NumberType val{number};

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
    else
    {
        return std::to_string(number);
    }

}

struct ClientAddress
{
    std::string ip;
    std::string port;
};

template <typename InputNumberType>
class TimeIntervalSumSession : public TCPSession
{
public:
    TimeIntervalSumSession(const std::shared_ptr<TCPServer>& server , size_t sumTimeIntervalInMilliseconds) :
        TimeIntervalSumSession(
             server
            , sumTimeIntervalInMilliseconds
            , std::make_shared<BacklogNumbersStreamParser<InputNumberType>>('\n', 32)
            , std::make_shared<TimeIntervalSumCalculator<InputNumberType>>(sumTimeIntervalInMilliseconds)
            , std::make_shared<StdTimeProvider>()
            , std::make_shared<StdLogger>())
    {}

    TimeIntervalSumSession(
          const std::shared_ptr<TCPServer>& server
         , size_t sumTimeIntervalInMilliseconds
         , INumbersStreamParserPtr<InputNumberType> parser
         , ITimeIntervalSumCalculatorPtr<InputNumberType> calculator
         , ITimeProviderPtr timeProvider
         , ILoggerPtr logger) :
            TCPSession(server)
            , m_timeProvider(std::move(timeProvider))
            , m_logger(std::move(logger))
            , m_sumCalculator(std::move(calculator))
            , m_parser(std::move(parser))
    {}

protected:
    void onConnected() override
    {
        m_client =
        {
              socket().remote_endpoint().address().to_string()
            , std::to_string(socket().remote_endpoint().port())
        };
        m_sessionStartTime = m_timeProvider->now();
    }

    void onReceived(const void* buffer, size_t size) override
    {
        try
        {
            if (auto prevOrNewData = m_parser->parse(buffer, size); prevOrNewData)
            {
                ProcessRequest(*prevOrNewData);
                while(auto val = m_parser->next())
                {
                    ProcessRequest(*val);
                }
            }
        }
        catch (const std::exception& exc)
        {
            std::string errorMsg = "An exception occurred.\n"
              "Message: " + std::string(exc.what())     +"\n"
              "Terminate session with ID " + m_sessionId +"\n";
            m_logger->error(errorMsg);
            Disconnect();
        }
        catch (...)
        {
            m_logger->error("Unknown error occurred.\nTerminate session with ID " + m_sessionId + "\n");
            Disconnect();
        }
    }

private:
    void ProcessRequest(InputNumberType value)
    {
        m_sumCalculator->put(value);
        auto curSumStr = fast_to_string(m_sumCalculator->get()) + "\n";

        if (!SendAsync(curSumStr.data(), curSumStr.size()))
        {
            throw TimeIntervalSumSessionException("Unexpected error occurred. "
                                                  "Can not send message to client. Socket is closed.");
        }

        auto secondsSinceStart = fast_to_string((m_timeProvider->now() - m_sessionStartTime) / 1000);
        auto msg = secondsSinceStart + ". Got " + fast_to_string(value) + " from " +
                   m_client.ip + ":" + m_client.port + "(session ID: " + m_sessionId + "). "
                   + "The current sum is " + curSumStr;
        m_logger->info(msg);
    }

protected:
    ClientAddress m_client;
    const std::string m_sessionId = id().string();
    const ITimeProviderPtr m_timeProvider;
    const ILoggerPtr m_logger;
    size_t m_sessionStartTime = -1;
    const ITimeIntervalSumCalculatorPtr<InputNumberType> m_sumCalculator;
    const INumbersStreamParserPtr<InputNumberType> m_parser;
};

#endif //TIME_INTERVAL_SUM_SERVER_TIMEINTERVALSUMSESSION_H
