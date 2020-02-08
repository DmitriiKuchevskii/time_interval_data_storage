#include "server/asio/service.h"
#include "system/cpu.h"
#include "server.h"

int main(int argc, char** argv)
{
    auto server = std::make_shared<TimeIntervalSumServer<int64_t>>(60 * 1000);
    std::cout << "Listening on port: " << server->port() << "\nPress any key to terminate the server.\n";
    server->Start();
    std::cin.get();
}