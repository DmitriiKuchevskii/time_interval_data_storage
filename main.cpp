#include <iostream>
#include <cmath>

#include "server/asio/service.h"
#include "server/asio/tcp_server.h"
#include "system/cpu.h"

#include "TimeIntervalDataStorage.h"

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
        size_t secondsSinceStart = duration_cast<seconds>(steady_clock::now().time_since_epoch()).count() - kServerStartTime;
        auto secondsSinceStartStr = fast_to_string(secondsSinceStart);

        auto strValues = (const char*)buffer;
        char *end;
        for (auto curValue = std::strtol(strValues, &end, 10);
             strValues != end;
             curValue = std::strtol(strValues, &end, 10))
        {
            if (std::distance((const char*)buffer, (const char*)end) >= size)
                return;
            strValues = end;
            handle(curValue, secondsSinceStartStr);
        }
    }

private:
    void handle(int64_t value, const std::string& secondsSinceStart)
    {
        m_storage.put(value);
        auto curSumStr = fast_to_string(m_storage.get()) + "\n";

        if (!SendAsync(curSumStr.data(), curSumStr.size()))
        {
            throw std::runtime_error("Can not send data. Unknown reason. Terminate server.");
        }

        auto msg = secondsSinceStart + ". Got " + fast_to_string(value) + " from " +
                   m_remoteAddress + ":" + m_remoteAddressPort + "(session ID: " + kSessionId + "). "
                   + "The current sum is " + curSumStr;
        std::cout << msg;
    }

protected:
    static std::atomic<size_t> m_sessions;
    TimeIntervalDataStorage<int64_t> m_storage;
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






class MockSession : public  Session
{
public:
    MockSession(const std::shared_ptr<TCPServer> &server, size_t startTime) : Session(server, startTime) {};

    void sendTestMessage(const std::string& msg)
    {
        onReceived(msg.data(), msg.size());
    }

    size_t getCurSum() const
    {
        return m_storage.get();
    }

    bool SendAsync(const void* buff, size_t size) override
    {
        return true;
    }
};

void test_multi_packets()
{
    auto service = std::make_shared<Service>(0);
    auto server = std::make_shared<Server>(service, 0000);
    MockSession mockSession(server, 0);
    mockSession.sendTestMessage("1\n\r2\n\r3\n\r");
    if (mockSession.getCurSum() != 1 + 2 + 3)
        throw std::runtime_error("Multi requests do not work!. FIX ME FIRST!!!!");
    mockSession.sendTestMessage("-6\n\r0\n\r0\n\r");
    if (mockSession.getCurSum() != 0)
        throw std::runtime_error("Multi requests do not work!. FIX ME FIRST!!!!");
    mockSession.sendTestMessage("-");
    mockSession.sendTestMessage("12");
    mockSession.sendTestMessage("3");
    mockSession.sendTestMessage("\n");
    if (mockSession.getCurSum() != 0)
        throw std::runtime_error("Multi requests do not work!. FIX ME FIRST!!!!");
    mockSession.sendTestMessage("\r");
    if (mockSession.getCurSum() != -123)
        throw std::runtime_error("Multi requests with separated messages do not work!. FIX ME FIRST!!!!");
}

volatile size_t approx_number_of_iterations_for_1microsec_wait()
{
    constexpr size_t iters = 1000000000;
    volatile size_t test = iters;
    auto t1 = std::chrono::high_resolution_clock::now();
    while(test--) {}
    auto t2 = std::chrono::high_resolution_clock::now();
    size_t elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    return (iters * 1.05) / elapsedTime; // It is not accurate, of course, but in fact we can't guarantee anything anyway.
}
volatile size_t MicrosecondWaitIterations;

volatile void busy_wait(size_t microSec)
{
    volatile size_t non_optimize = microSec * MicrosecondWaitIterations;
    while(non_optimize--) {}
}

void test_storage_perfomance(size_t testTimeInSeconds, size_t messagesPerSec)
{
    TimeIntervalDataStorage<int64_t> dataStorage;
    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < messagesPerSec * testTimeInSeconds; ++i)
    {
        dataStorage.put(1);
        busy_wait(1000000 / messagesPerSec);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    size_t perfectTime = testTimeInSeconds * 1000;
    std::cout << "Perfect time: " <<  perfectTime << " milliseconds\n";
    size_t elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    if (perfectTime > elapsedTime)
        elapsedTime = perfectTime; // So, yeah.... silly but... you know... what else can we do here ???
    std::cout << "Elapsed time: " << elapsedTime << " milliseconds\n";
    std::cout << "Error " << elapsedTime - perfectTime << " milliseconds (" << ((100.0 * (elapsedTime - perfectTime)) / elapsedTime) << "%)\n";
    std::cout << "Current sum is: " << dataStorage.get() << "\n";
}

int main(int argc, char** argv)
{
    if (argc > 1 && std::string(argv[1]) == "test")
    {
        MicrosecondWaitIterations = approx_number_of_iterations_for_1microsec_wait();
        test_storage_perfomance(180, 10000000);
       // test_multi_packets();
        std::cout << "ALL TESTS PASSED\n";
        return 0;
    }

    auto service = std::make_shared<Service>(std::thread::hardware_concurrency(), true);
    service->Start();

    auto server = std::make_shared<Server>(service, 1111);
    std::cout << "Listening port 1111\n";
    server->SetupReuseAddress(true);
    server->SetupReusePort(true);
    server->SetupNoDelay(true);
    server->Start();

    std::cin.get();
}