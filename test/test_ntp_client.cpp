// Description: Test for ntp_client.cpp
#include <iostream>
#include <array>
#include <gtest/gtest.h>
#include "mock_ntp_client.hpp"
#include "ntp_client_api.hpp"

class TestNtpClientTestFixture : public ::testing::TestWithParam<std::array<int, 4>>
{
protected :
    void SetUp() override
    {
        std::cout << "TestNtpClientTestFixture::SetUp() called\n";         
    }
    MockNtpClient mock_ntp_client_;
};

TEST_P(TestNtpClientTestFixture, healthy_connection) 
{   
    using testing::Return;

    constexpr std::uint32_t expected_time{300U};
    // calls_times_expectations
    const auto calls_times_expectations = GetParam();

    switch (calls_times_expectations[0])
    {
        case 0:
            EXPECT_CALL(mock_ntp_client_, createConnection()).Times(0);
            break;
        case 1:
            EXPECT_CALL(mock_ntp_client_, createConnection()).Times(1).WillOnce(Return(std::expected<void, std::string>{}));
            break;        
        case 2:
        default:
            EXPECT_CALL(mock_ntp_client_, createConnection()).Times(calls_times_expectations[0]).WillOnce(Return(std::unexpected("Error: Any error")));
            break;
       
    }
    if (calls_times_expectations[1]) EXPECT_CALL(mock_ntp_client_, sendRequest()).Times(calls_times_expectations[1]).WillOnce(Return(std::expected<void, std::string>{}));
    else EXPECT_CALL(mock_ntp_client_, sendRequest()).Times(calls_times_expectations[1]).WillOnce(Return(std::unexpected("Error: Any error")));

    if (calls_times_expectations[2]) EXPECT_CALL(mock_ntp_client_, receiveResponse()).Times(calls_times_expectations[2]).WillOnce(Return(std::expected<std::uint32_t, std::string>{expected_time}));
    else EXPECT_CALL(mock_ntp_client_, receiveResponse()).Times(calls_times_expectations[2]).WillOnce(Return(std::unexpected("Error: Any error")));

    if (calls_times_expectations[3]) EXPECT_CALL(mock_ntp_client_, cleanupConnection()).Times(calls_times_expectations[3]).WillOnce(Return(std::expected<void, std::string>{}));
    else EXPECT_CALL(mock_ntp_client_, cleanupConnection()).Times(calls_times_expectations[3]).WillOnce(Return(std::unexpected("Error: Any error")));

    NTPClientApi ntp_client_api{"domain.com", 123U, &mock_ntp_client_};

    const auto val_s = ntp_client_api.request_time();

    ASSERT_TRUE(val_s.has_value());
    ASSERT_EQ(val_s.value(), expected_time);
}

INSTANTIATE_TEST_CASE_P(
        NTPClientTests,
        TestNtpClientTestFixture,
        ::testing::Values(
                // calls_times_expectations: 1: call expected. 0: no call expected.
                // createConnection, sendRequest, receiveResponse, cleanupConnection
                //std::array<int,4>{1,1,1,0}
                std::array<int,4>{2,0,0,1}
                // std::array<int,4>{1,0,1,1},
                // std::array<int,4>{1,1,0,1}
        ));