#include <cpp-optparse/OptionParser.h>
#include "server/asio/service.h"
#include "system/cpu.h"
#include "MultiSessionServer.h"
#include "TimeIntervalSumSession.h"

template <typename Server>
void start_server(const std::shared_ptr<Server>& server, const std::string& startMsg)
{
    std::cout << startMsg << std::endl;

    server->Start();

    std::cout << "Listening on port: " << server->port() << std::endl
              << "Press ENTER to terminate the server."  << std::endl;

    std::cin.get();
}

int main(int argc, char** argv)
{
    try
    {
        auto parser = optparse::OptionParser().version("1.0.0.0");

        parser.add_option("-p", "--port").dest("port").action("store").type("int").set_default(1111).help(
                "Server port. Default: %default");

        parser.add_option("-t", "--time").dest("time").action("store").type("int").set_default(60 * 1000).help(
                "Time interval for sum in milliseconds. Default: %default");

        parser.add_option("-i", "--inputtype").dest("type").action("store")
               .choices({"integer", "double"}) .set_default("integer").help("Input numbers type. Default: %default");

        optparse::Values options = parser.parse_args(argc, argv);

        auto service = std::make_shared<Service>(std::thread::hardware_concurrency(), true);

        int port = options.get("port");
        size_t time = std::max((size_t) options.get("time"), 1ul);

        std::cout << "Time interval for sum is " << time << " milliseconds." << std::endl;

        if (std::string type = options["type"]; type == "integer")
        {
            start_server(
                  make_multi_session_server<TimeIntervalSumSession<int64_t>>(service, port, time)
                , "Integers-only mode."
            );
        }
        else if (type == "double")
        {
            start_server(
                   make_multi_session_server<TimeIntervalSumSession<double>>(service, port, time)
                , "Floating point numbers mode."
            );
        }
    }
    catch (int optparseErrorCode)
    {
        return optparseErrorCode;
    }

    std::cout << "The server has been successfully shut down.\n";
}