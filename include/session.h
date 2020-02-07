#pragma once
//
// Created by dkuch on 07.02.2020.
//

#ifndef TIME_INTERVAL_SUM_SERVER_SESSION_H
#define TIME_INTERVAL_SUM_SERVER_SESSION_H


#include "server/asio/service.h"
#include "server/asio/tcp_server.h"
#include "system/cpu.h"

#include "TimeIntervalSumCalculator.h"

using namespace CppCommon;
using namespace CppServer::Asio;


struct ClientAddress
{
    std::string ip;
    std::string port;
};

template <typename InputNumberType, typename TimeProvider = StdTimeProviderr>
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
    }

protected:
    ClientAddress m_client;
    const std::string KSessionId;
    const size_t kSessionStartTime = TimeProvider::Now();
    TimeIntervalSumCalculator<InputNumberType, TimeProvider> m_sumCalculator;
};

#endif //TIME_INTERVAL_SUM_SERVER_SESSION_H
