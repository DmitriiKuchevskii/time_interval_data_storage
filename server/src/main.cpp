#include <cpp-optparse/OptionParser.h>
#include "server/asio/service.h"
#include "system/cpu.h"
#include "MultiSessionServer.h"
#include "TimeIntervalSumSession.h"

int main(int argc, char** argv)
{
    try
    {
        auto parser = optparse::OptionParser().version("1.0.0.0");

        parser.add_option("-p", "--port").dest("port").action("store").type("int").set_default(1111).help(
                "Server port. Default: %default");

        parser.add_option("-t", "--time").dest("time").action("store").type("int").set_default(60 * 1000).help(
                "Time interval for sum in milliseconds. Default: %default");

        parser.add_option("-i", "--inputtype").dest("type").action("store").set_default("integer").help(
                "Input numbers type. Default: %default");

        optparse::Values options = parser.parse_args(argc, argv);

        auto service = std::make_shared<Service>(std::thread::hardware_concurrency(), true);

        int port = options.get("port");
        int time = std::max((int) options.get("time"), 1);

        std::cout << "Time interval for sum is " << time << " milliseconds\n";

        if (std::string type = options["type"]; type == "integer")
        {
            auto integer_server = make_multi_session_server<TimeIntervalSumSession<int64_t>>(service, port, time);
            std::cout << "Integers-only mode.\n";
            integer_server->Start();
        }
        else if (type == "double")
        {
            auto double_server = make_multi_session_server<TimeIntervalSumSession<double>>(service, port, time);
            std::cout << "Floating numbers mode.\n";
            double_server->Start();
        }
        else
        {
            std::cerr << "Type '" << type << "' is not supported\n";
            exit(EXIT_FAILURE);
        }

        std::cout << "Listening on port: " << port << "\nPress ENTER to terminate the server.\n";
    }
    catch (int optparserErrorCode)
    {
        return optparserErrorCode;
    }

    std::cin.get();
    std::cout << "The server has been successfully shut down.\n";
}