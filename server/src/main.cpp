#include "server/asio/service.h"
#include "system/cpu.h"
#include "MultiSessionServer.h"
#include "TimeIntervalSumSession.h"

int main(int argc, char** argv)
{
    auto service = std::make_shared<Service>(std::thread::hardware_concurrency(), true);
    int port = 1111;
    auto server = make_multi_session_server<TimeIntervalSumSession<int64_t>>(service, port, 60 * 1000);
    std::cout << "Listening on port: " << server->port() << "\nPress any key to terminate the server.\n";
    server->Start();
    std::cin.get();
}