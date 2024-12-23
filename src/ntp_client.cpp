#include "ntp_client.hpp"
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <sys/types.h>

#ifdef _WIN32
    #include <WinSock2.h>
    #define close(X) closesocket(X)
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
#endif

#include <time.h>
#include <string.h>

namespace {
    /// @brief Delta between epoch time and ntp time
    constexpr std::uint32_t NTP_TIMESTAMP_DELTA{2208988800U};

    /**
     * @brief Converts from hostname to ip address
     * 
     * @param hostname name of the host.
     * @return ip address. Return empty string if no IP found.
     */
    std::string hostname_to_ip(const std::string& host)
    {
        hostent *hostname = gethostbyname(host.c_str());// this function is deprecated. TODO: replace it.
        if (hostname)
            return std::string(inet_ntoa(**(in_addr **)hostname->h_addr_list));
        return {};
    }
}

NTPClientApi::~NTPClientApi()
{
}

std::expected<uint32_t, std::string> NTPClientApi::request_time()
{
    return ntp_client.createConnection().and_then([&]() -> std::expected<void, std::string> {
        const auto maybe_packet = ntp_client.sendRequest();
        if (!maybe_packet.has_value()) return std::unexpected("Error: writing to socket");
        else return {};
    }).and_then([&]() -> std::expected<uint32_t, std::string> {
        const auto maybe_abs_seconds = ntp_client.receiveResponse();        
        if (!maybe_abs_seconds.has_value()) return std::unexpected("Error: reading from socket");        
        else return maybe_abs_seconds.value();
    }).or_else([&](const std::string err) -> std::expected<uint32_t, std::string> {        
        auto e = ntp_client.cleanupConnection();
        if (!e.has_value()) return std::unexpected("Error: closing socket");
        else return std::unexpected(err);
    });
}

NtpClient::~NtpClient()
{
    cleanupConnection();
}

std::expected<void, std::string> NtpClient::createConnection()
{   
    #ifdef _WIN32
    WSADATA wsaData = { 0 };
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData)) return std::unexpected("Error: WSAStartup failed");
    #endif
    
    // Creating socket file descriptor
    if ((si_.socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        std::cerr << "Socket creation failed\n";
        return std::unexpected("Error: Socket creation failed");
    }

    memset(&si_.socket_client, 0, sizeof(si_.socket_client));
    std::string ntp_server_ip = hostname_to_ip(hostname_);
    std::cout << "NTP server IP: " << ntp_server_ip << "\n";

#ifdef _WIN32
    DWORD timeout_time_value =1000;// timeout in 1 second in ms
#else
    timeval timeout_time_value{1, 0};// timeout in 1 seconds + 0us
#endif

    setsockopt(si_.socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_time_value, sizeof(timeout_time_value));

    // Filling server information
    si_.socket_client.sin_family = AF_INET;
    si_.socket_client.sin_port = htons(port_);
    si_.socket_client.sin_addr.s_addr = inet_addr(ntp_server_ip.c_str());

    if (connect(si_.socket_fd, (struct sockaddr *)&si_.socket_client, sizeof(si_.socket_client)) < 0)
    {
        return std::unexpected("Error: Socket creation failed");
    }

    return {};
}

std::expected<void, std::string> NtpClient::sendRequest()
{
    NtpPacket packet = NtpPacketFactory{}.getPacket();

    #ifdef _WIN32
        const auto sending_error = sendto(si_.socket_fd, (char*)&packet, sizeof(NtpPacket), 0, (struct sockaddr*)&si_.socket_client, sizeof(si_.socket_client));
    #else
        const auto sending_error = write(si_.socket_fd, (char*)&packet, sizeof(NtpPacket));
    #endif

    if (sending_error < 0) return std::unexpected("Error: writing to socket");
    else return {};
}

std::expected<std::uint32_t, std::string> NtpClient::receiveResponse()
{
    NtpPacket packet = NtpPacketFactory{}.getPacket();

    #ifdef _WIN32
        const auto reading_error = recv(si_.socket_fd, (char*)&packet, sizeof(NtpPacket), 0);
    #else
        const auto reading_error = read(si_.socket_fd, (char*)&packet, sizeof(NtpPacket));
    #endif

    if (reading_error < 0) return std::unexpected("Error: reading from socket");
    else {
        
        // These two fields contain the time-stamp seconds as the packet left the NTP
        // server. The number of seconds correspond to the seconds passed since 1900.
        // ntohl() converts the bit/byte order from the network's to host's
        // "endianness".

        packet.transmitted_timestamp_sec = ntohl(packet.transmitted_timestamp_sec);           // Time-stamp seconds.
        packet.transmitted_timestamp_sec_frac = ntohl(packet.transmitted_timestamp_sec_frac); // Time-stamp fraction of a second.

        // Extract the 32 bits that represent the time-stamp seconds (since NTP epoch)
        // from when the packet left the server. Subtract 70 years worth of seconds
        // from the seconds since 1900. This leaves the seconds since the UNIX epoch
        // of 1970.
        // (1900)---------(1970)**********(Time Packet Left the Server)

        // seconds since UNIX epoch
        uint32_t txTm = packet.transmitted_timestamp_sec - NTP_TIMESTAMP_DELTA;
        // // convert seconds to milliseconds
        // uint64_t milliseconds = (uint64_t)txTm * 1000l;

        return txTm;
    }   
}

std::expected<void, std::string> NtpClient::cleanupConnection()
{
    if (si_.socket_fd != -1)
    {
        if (0 != close(si_.socket_fd)) return std::unexpected("Error: closing socket");
        si_.socket_fd = -1;
    }

    #ifdef _WIN32
    WSACleanup();
    #endif

    return {};
}