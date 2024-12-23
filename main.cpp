#include "ntp_client.hpp"

#include <iostream>
#include <iomanip>

#include "time.h"

int main(int argc, char *argv[])
{
    // if (argc < 3)
    // {
    //     std::cout << "Usage: " << argv[0] << " \"Address of NTP server pool\" \"port\" \n";

    //     std::cout << "Example: \n\t";
    //     std::cout << argv[0] << " 0.pool.ntp.org 123\n";

    //     return EXIT_FAILURE;
    // }

    // uint16_t port = std::stoi(argv[2]);

    //NTPClientApi client{argv[1], port};
   
    NTPClientApi client{"0.pool.ntp.org", 123U};

    const auto maybe_time = client.request_time();

    if (!maybe_time.has_value())
        return EXIT_FAILURE;

    // The function ctime receives the timestamps in seconds.
    time_t epoch_server = maybe_time.value();
    
    std::cout << "Server time: " << ctime(&epoch_server);
    std::cout << "Timestamp server: " << (uint32_t)epoch_server << "\n\n";

    time_t local_time;
    local_time = time(0);

    std::cout << "System time is " << (epoch_server - local_time) << " seconds off\n";

    return EXIT_SUCCESS;
}