#pragma once 

#include "INtpClient.hpp"
#include "ntp_client.hpp"

class NTPClientApi
{
public:
    // p_ntp_client allows to inject a mock object for testing
    explicit NTPClientApi(const std::string hostname, const uint16_t port, INtpClient* const p_ntp_client=nullptr) : ntp_client(hostname, port){
        if (p_ntp_client != nullptr) ntp_client_ = p_ntp_client;
        else ntp_client_ = &ntp_client;
    };

    /**
   * @brief Transmits an NTP request to the defined server and returns the
   * timestamp
   *
   * @return std::expected<uint32_t, std::string> the number of seconds since 1970. Return Error in string if fail. 
   */
    std::expected<uint32_t, std::string> request_time() const {
        return ntp_client_->createConnection().and_then([&]() -> std::expected<void, std::string> {
            const auto maybe_packet = ntp_client_->sendRequest();
            if (!maybe_packet.has_value()) return std::unexpected("Error: writing to socket");
            else return {};
        }).and_then([&]() -> std::expected<uint32_t, std::string> {
            const auto maybe_abs_seconds = ntp_client_->receiveResponse();        
            if (!maybe_abs_seconds.has_value()) return std::unexpected("Error: reading from socket");        
            else return maybe_abs_seconds.value();
        }).or_else([&](const std::string err) -> std::expected<uint32_t, std::string> {        
            auto e = ntp_client_->cleanupConnection();
            if (!e.has_value()) return std::unexpected("Error: closing socket");
            else return std::unexpected(err);
        });
    }


private:    
    INtpClient* ntp_client_;// points to the mock object if it is injected or the default implementation
    NtpClient ntp_client;// default implementation
};


