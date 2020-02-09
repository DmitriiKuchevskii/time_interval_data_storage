#include <cpp-optparse/OptionParser.h>
#include "server/asio/service.h"
#include "system/cpu.h"
#include "MultiSessionServer.h"
#include "TimeIntervalSumSession.h"

int main(int argc, char** argv)
{
    auto parser = optparse::OptionParser().version("1.0.0.0");
    parser.add_option("-p", "--port").dest("port").action("store").type("int").set_default(1111).help("Server port. Default: %default");
    optparse::Values options = parser.parse_args(argc, argv);

    auto service = std::make_shared<Service>(std::thread::hardware_concurrency(), true);
    int port = options.get("port");
    auto server = make_multi_session_server<TimeIntervalSumSession<int64_t>>(service, port, 60 * 1000);
    std::cout << "Listening on port: " << server->port() << "\nPress any key to terminate the server.\n";
    server->Start();
    std::cin.get();
    std::cout << "ALL DONE!";
}