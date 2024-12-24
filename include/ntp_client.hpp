#pragma once 


#ifdef _WIN32
#include <WinSock2.h>
using Socket = SOCKET;
#else
#include <netinet/in.h>
using Socket = int;
#endif
#include <expected>
#include <string>
#include "INtpClient.hpp"

struct NtpPacket
{    
    uint8_t li_vn_mode; // Eight bits. li, vn, and mode.
                        // li.   Two bits.   Leap indicator.
                        // vn.   Three bits. Version number of the protocol.
                        // mode. Three bits. Client will pick mode 3 for client.
    uint8_t stratum;// Eight bits. Stratum level of the local clock.    
    uint8_t poll;// Eight bits. Maximum interval between successive messages.
    uint8_t precision;// Eight bits. Precision of the local clock.   
    uint32_t rootDelay;// 32 bits. Total round trip delay time.  
    uint32_t root_dispersion;// 32 bits. Max error aloud from primary clock source.
    uint32_t ref_id;// 32 bits. Reference clock identifier.
    uint32_t ref_timestamp_sec;// 32 bits. Reference time-stamp seconds.
    uint32_t ref_timestamp_sec_frac; // 32 bits. Reference time-stamp fraction of a second.
    uint32_t orig_timestamp_sec;// 32 bits. Originate time-stamp seconds.
    uint32_t orig_timestamp_sec_frac;// 32 bits. Originate time-stamp fraction of a second.   
    uint32_t received_timestamp_sec;// 32 bits. Received time-stamp seconds.
    uint32_t received_timestamp_sec_frac;// 32 bits. Received time-stamp fraction of a second.
    uint32_t transmitted_timestamp_sec;// 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
    uint32_t transmitted_timestamp_sec_frac;// 32 bits. Transmit time-stamp fraction of a second.
};
class NtpPacketFactory
{
public:
    // LI   = 0    (Leap indicator) 2 bits
    // VN   = 4    (Version number) 3 bits
    // Mode = 3    (Mode, mode 3 is client mode) 3 bits
    NtpPacket getPacket() {
        NtpPacket packet = {};   // LI  VN  Mode
        packet.li_vn_mode = 0x23;// b00 100 011
        return packet;// NVO
    };
};

struct SocketInfo
{
    Socket socket_fd;
    struct sockaddr_in socket_client;
};

class NtpClient final : public INtpClient
{
public:
    explicit NtpClient(const std::string host, const uint16_t port) : hostname_(host), port_(port){};
    ~NtpClient();

    std::expected<void, std::string> createConnection() override;
    std::expected<void, std::string> sendRequest() override;
    std::expected<std::uint32_t, std::string> receiveResponse() override;
    std::expected<void, std::string> cleanupConnection() override;

private:
    std::string hostname_;
    uint16_t port_;
    SocketInfo si_{-1, {}};// initisalize socket_fd to -1 as invalid socket
};


