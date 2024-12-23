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
    static constexpr unsigned long long NTP_TIMESTAMP_DELTA{2208988800ull};

    /**
     * @brief Converts from hostname to ip address
     * 
     * @param hostname name of the host.
     * @return ip address. Return empty string if coun't find the ip.
     */
    std::string hostname_to_ip(const std::string& host)
    {
        hostent *hostname = gethostbyname(host.c_str());// this function is deprecated. TODO: replace it.
        if (hostname)
            return std::string(inet_ntoa(**(in_addr **)hostname->h_addr_list));
        return {};
    }
}

NTPClientApi::NTPClientApi(const std::string hostname, const uint16_t port) : hostname_(hostname), port_(port), ntp_client(hostname, port)
{
#ifdef _WIN32
    WSADATA wsaData = { 0 };
    (void)WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

NTPClientApi::~NTPClientApi()
{
    close_socket();

#ifdef _WIN32
    WSACleanup();
#endif
}

std::expected<uint32_t, std::string> NTPClientApi::request_time()
{
    return ntp_client.createConnection().and_then([&](const auto si) -> std::expected<SocketInfo, std::string> {
        const auto maybe_packet = ntp_client.sendRequest(si);
        if (!maybe_packet.has_value()) return std::unexpected("Error: writing to socket");
        else return si;
    }).and_then([&](const auto si) -> std::expected<uint32_t, std::string> {
        const auto maybe_abs_seconds = ntp_client.receiveResponse(si);        
        if (!maybe_abs_seconds.has_value()) return std::unexpected("Error: reading from socket");        
        else return maybe_abs_seconds.value();
    });
}

void NTPClientApi::close_socket()
{
    if (socket_fd != -1)
    {
        close(socket_fd);
        socket_fd = -1;
    }
}

NTPClient::NTPClient(const std::string host, const uint16_t port) : hostname_(host), port_(port)
{
}

std::expected<SocketInfo, std::string> NTPClient::createConnection()
{   
    auto si = SocketInfo{};
    
    // Creating socket file descriptor
    if ((si.socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        std::cerr << "Socket creation failed\n";
        return std::unexpected("Error: Socket creation failed");
    }

    memset(&si.socket_client, 0, sizeof(si.socket_client));

    std::string ntp_server_ip = hostname_to_ip(hostname_);

    std::cout << "NTP server IP: " << ntp_server_ip << "\n";

#ifdef _WIN32
    DWORD timeout_time_value =1000;// timeout in 1 second in ms
#else
    timeval timeout_time_value{1, 0};// timeout in 1 seconds + 0us
#endif

    setsockopt(si.socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_time_value, sizeof(timeout_time_value));

    // Filling server information
    si.socket_client.sin_family = AF_INET;
    si.socket_client.sin_port = htons(port_);
    si.socket_client.sin_addr.s_addr = inet_addr(ntp_server_ip.c_str());

    std::cout << "Connecting\n";
    if (connect(si.socket_fd, (struct sockaddr *)&si.socket_client, sizeof(si.socket_client)) < 0)
    {
        return std::unexpected("Error: Socket creation failed");
    }

    return si;
}

std::expected<NtpPacket, std::string> NTPClient::sendRequest(const SocketInfo& si)
{
    NtpPacket packet = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    packet.li_vn_mode = 0x1b;

    std::cout << "Sending request\n";
    #ifdef _WIN32
        const auto sending_error = sendto(si.socket_fd, (char*)&packet, sizeof(NtpPacket), 0, (struct sockaddr*)&si.socket_client, sizeof(si.socket_client));
    #else
        const auto sending_error = write(si.socket_fd, (char*)&packet, sizeof(NtpPacket));
    #endif

    if (sending_error < 0)
    {
        std::cerr << "ERROR writing to socket\n";
        return std::unexpected("Error: writing to socket");
    } else {
        return packet;
    }
}

std::expected<std::uint32_t, std::string> NTPClient::receiveResponse(const SocketInfo& si)
{
    NtpPacket packet = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    packet.li_vn_mode = 0x1b;

    std::cout << "Reading request\n";
    #ifdef _WIN32
        const auto reading_error = recv(si.socket_fd, (char*)&packet, sizeof(NtpPacket), 0);
    #else
        const auto reading_error = read(si.socket_fd, (char*)&packet, sizeof(NtpPacket));
    #endif

    if (reading_error < 0)
    {
        std::cerr << "ERROR reading from socket\n";
        return std::unexpected("Error: reading from socket");
    } else {
        
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
