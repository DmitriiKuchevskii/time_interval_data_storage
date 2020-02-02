#include <iostream>

#include "server/asio/service.h"
#include "server/asio/tcp_server.h"
#include "system/cpu.h"

#include "TimeIntervalDataStorage.h"

using namespace CppCommon;
using namespace CppServer::Asio;

class Session : public TCPSession
{
public:
    Session(const std::shared_ptr<TCPServer> &server, size_t startTime) :
          TCPSession(server)
        , kServerStartTime(startTime)
    {}

protected:
    void onConnected() override
    {
        m_remoteAddress = socket().remote_endpoint().address().to_string();
        m_remoteAddressPort = std::to_string(socket().remote_endpoint().port());
    }

    void onReceived(const void* buffer, size_t size) override
    {
        if (size < 3)
            return;

        auto strValue = std::string((const char*)buffer, size - 2);
        size_t value = std::stoll(strValue);
        m_storage.put(value);
        std::string res = std::to_string(m_storage.get()) + "\n";

        if (!SendAsync(res.c_str(), res.size()))
            throw std::runtime_error("Can not send data. Unknown reason. Terminate server.");

        size_t secondsSinceStart = duration_cast<seconds>(steady_clock::now().time_since_epoch()).count() - kServerStartTime;
        auto msg = std::to_string(secondsSinceStart) + ". Got " + strValue + " from " +
                   m_remoteAddress + ":" + m_remoteAddressPort + "(session ID: " + kSessionId + "). "
                   + "The current sum is " + res;
        std::cout << msg;
    }

private:
    static std::atomic<size_t> m_sessions;
    TimeIntervalDataStorage m_storage{60 * 1000};
    const size_t kServerStartTime;
    std::string m_remoteAddress;
    std::string m_remoteAddressPort;
    const std::string kSessionId = std::to_string(m_sessions++);
};
std::atomic<size_t> Session::m_sessions {0};


class Server : public TCPServer
{
public:
    using TCPServer::TCPServer;

protected:
    std::shared_ptr<TCPSession> CreateSession(const std::shared_ptr<TCPServer>& server) override
    {
        return std::make_shared<Session>(server, StartTime);
    }

private:
    const size_t StartTime = duration_cast<seconds>(steady_clock::now().time_since_epoch()).count();
};

int main(int argc, char** argv)
{
    auto service = std::make_shared<Service>(std::thread::hardware_concurrency());
    service->Start();

    auto server = std::make_shared<Server>(service, 1111);
    std::cout << "Listening port 1111\n";
    server->SetupReuseAddress(true);
    server->SetupReusePort(true);
    server->SetupNoDelay(true);
    server->Start();

    std::cin.get();
}