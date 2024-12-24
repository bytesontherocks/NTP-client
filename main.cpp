#include "ntp_client_api.hpp"

#include <iostream>
#include <iomanip>

#include "time.h"
#include <thread>
#include <chrono>

int main(int argc, char *argv[])
{
    constexpr char default_ntp_domain[]{"0.pool.ntp.org"};
    constexpr uint16_t default_ntp_port{123U};

    std::string ntp_domain{default_ntp_domain}; 
    uint16_t ntp_port{default_ntp_port};    

    if (argc < 3)
    {
        std::cout << "Using default NTP server {" << default_ntp_domain << "} and port {" << default_ntp_port << "}\n";
        std::cout << "User can provide specific ones using args: " << argv[0] << " \"Address of NTP server pool\" \"port\" \n";
    } else {
        ntp_domain = argv[1];
        char* p;
        const auto res = std::strtol(argv[2], &p, 10);
        if (*p != '\0' || res < 0 || res > std::numeric_limits<uint16_t>::max())
        {
            std::cerr << "Invalid port number. It needs to be uint16_t\n";
            return EXIT_FAILURE;
        }
        ntp_port =res;
    }
   
    NTPClientApi client{ntp_domain, ntp_port};

    while(1)
    {
        const auto maybe_time = client.request_time();

        if (!maybe_time.has_value())
        {    
            std::cerr << maybe_time.error() << "\n";
            return EXIT_FAILURE;
        }

        // The function ctime receives the timestamps in seconds.
        time_t epoch_server = maybe_time.value();

        std::cout << "Server time: " << ctime(&epoch_server);
        std::cout << "Timestamp server: " << (uint32_t)epoch_server << "\n\n";

        time_t local_time;
        local_time = time(0);

        std::cout << "System time is " << (epoch_server - local_time) << " seconds off\n";

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    

    return EXIT_SUCCESS;
}