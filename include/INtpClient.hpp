#pragma once

#include <string>
#include <expected>
#include <cstdint>

// forward declaration
class SocketInfo;// socket_fd + socket_client
class NtpPacket;

class INtpClient
{
public:
    virtual ~INtpClient() = default;

    virtual std::expected<SocketInfo, std::string> createConnection() = 0;
    virtual std::expected<NtpPacket, std::string> sendRequest(const SocketInfo& si) = 0;
    virtual std::expected<std::uint32_t, std::string> receiveResponse(const SocketInfo& si) = 0;
};