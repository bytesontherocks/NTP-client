#pragma once

#include <string>
#include <expected>
#include <cstdint>

class INtpClient
{
public:
    virtual ~INtpClient() = default;

    virtual std::expected<void, std::string> createConnection() = 0;
    virtual std::expected<void, std::string> sendRequest() = 0;
    virtual std::expected<std::uint32_t, std::string> receiveResponse() = 0;
    virtual std::expected<void, std::string> cleanupConnection() = 0;
};