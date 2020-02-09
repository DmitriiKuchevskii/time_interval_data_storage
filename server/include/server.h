#pragma once
//
// Created by dkuch on 07.02.2020.
//
#ifndef TIME_INTERVAL_SUM_SERVER_SERVER_H
#define TIME_INTERVAL_SUM_SERVER_SERVER_H

#include "session.h"

template <typename SessionType, typename... SessionArgs>
class MultiSessionServer : public TCPServer
{
public:
    MultiSessionServer(const std::shared_ptr<Service>& service, int port, SessionArgs... sessionArgs) :
        TCPServer(service, port)
       , m_sessionArgs(std::make_tuple(sessionArgs...))
    {
        this->service()->Start();
        SetupReuseAddress(true);
        SetupReusePort(true);
        SetupNoDelay(true);
    }

protected:
    std::shared_ptr<TCPSession> CreateSession(const std::shared_ptr<TCPServer>& server) override
    {
        return std::apply(std::make_shared<SessionType, const std::shared_ptr<TCPServer>&, SessionArgs...>,
                          std::tuple_cat(std::make_tuple(server), std::move(m_sessionArgs))
                          );
    }

private:
    std::tuple<SessionArgs...> m_sessionArgs;
};

template <typename SessionType, typename... SessionArgs>
auto make_multi_session_server(const std::shared_ptr<Service>& service, int port, SessionArgs... args)
{
    return std::make_shared<MultiSessionServer<SessionType, SessionArgs...>>(service, port, args...);
}

#endif //TIME_INTERVAL_SUM_SERVER_SERVER_H
