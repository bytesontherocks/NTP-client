// Description: Test for ntp_client.cpp
#include <iostream>
#include <array>
#include <gtest/gtest.h>
#include "mock_ntp_client.hpp"
#include "ntp_client_api.hpp"

using testing::Return;

TEST(TestNtpClient, healthy_connection) 
{   
  
    constexpr std::uint32_t expected_time{300U};
    MockNtpClient mock_ntp_client{};

    EXPECT_CALL(mock_ntp_client, createConnection()).Times(1).WillOnce(Return(std::expected<void, std::string>{}));
    EXPECT_CALL(mock_ntp_client, sendRequest()).Times(1).WillOnce(Return(std::expected<void, std::string>{}));
    EXPECT_CALL(mock_ntp_client, receiveResponse()).Times(1).WillOnce(Return(std::expected<std::uint32_t, std::string>{expected_time}));
    EXPECT_CALL(mock_ntp_client, cleanupConnection()).Times(0);

    NTPClientApi ntp_client_api{"domain.com", 123U, &mock_ntp_client};

    const auto val_s = ntp_client_api.request_time();

    ASSERT_TRUE(val_s.has_value());
    ASSERT_EQ(val_s.value(), expected_time);
}

TEST(TestNtpClient, connection_creation_fails) 
{   
  
    constexpr std::uint32_t expected_time{300U};
    MockNtpClient mock_ntp_client{};

    EXPECT_CALL(mock_ntp_client, createConnection()).Times(1).WillOnce(Return(std::unexpected("Error: Socket creation failed")));
    EXPECT_CALL(mock_ntp_client, sendRequest()).Times(0);
    EXPECT_CALL(mock_ntp_client, receiveResponse()).Times(0);
    EXPECT_CALL(mock_ntp_client, cleanupConnection()).Times(1);

    NTPClientApi ntp_client_api{"domain.com", 123U, &mock_ntp_client};

    const auto val_s = ntp_client_api.request_time();

    ASSERT_FALSE(val_s.has_value());
}

TEST(TestNtpClient, send_requests_fails) 
{   
  
    constexpr std::uint32_t expected_time{300U};
    MockNtpClient mock_ntp_client{};

    EXPECT_CALL(mock_ntp_client, createConnection()).Times(1).WillOnce(Return(std::expected<void, std::string>{}));
    EXPECT_CALL(mock_ntp_client, sendRequest()).Times(1).WillOnce(Return(std::unexpected("Error: writing to socket")));
    EXPECT_CALL(mock_ntp_client, receiveResponse()).Times(0);
    EXPECT_CALL(mock_ntp_client, cleanupConnection()).Times(1);

    NTPClientApi ntp_client_api{"domain.com", 123U, &mock_ntp_client};

    const auto val_s = ntp_client_api.request_time();

    ASSERT_FALSE(val_s.has_value());
}

TEST(TestNtpClient, receive_response_fails) 
{   
  
    constexpr std::uint32_t expected_time{300U};
    MockNtpClient mock_ntp_client{};

    EXPECT_CALL(mock_ntp_client, createConnection()).Times(1).WillOnce(Return(std::expected<void, std::string>{}));
    EXPECT_CALL(mock_ntp_client, sendRequest()).Times(1).WillOnce(Return(std::expected<void, std::string>{}));  
    EXPECT_CALL(mock_ntp_client, receiveResponse()).Times(1).WillOnce(Return(std::unexpected("Error: reading from socket")));
    EXPECT_CALL(mock_ntp_client, cleanupConnection()).Times(1);

    NTPClientApi ntp_client_api{"domain.com", 123U, &mock_ntp_client};

    const auto val_s = ntp_client_api.request_time();

    ASSERT_FALSE(val_s.has_value());
}