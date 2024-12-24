#pragma once

#include <gmock/gmock.h>  // Brings in gMock.
#include "INtpClient.hpp"
#include <expected>
#include <string>

class MockNtpClient : public INtpClient {
public:
    MOCK_METHOD((std::expected<void, std::string>), createConnection, (), (override));
    MOCK_METHOD((std::expected<void, std::string>), sendRequest, (), (override));
    MOCK_METHOD((std::expected<std::uint32_t, std::string>), receiveResponse, (), (override));
    MOCK_METHOD((std::expected<void, std::string>), cleanupConnection, (), (override));
};
