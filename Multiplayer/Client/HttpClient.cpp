#include "HttpClient.h"
#include <iostream>
#include <cpr/cpr.h>  // Include cpr library for HTTP requests

HttpClient::HttpClient(const std::string& serverUrl)
    : serverUrl(serverUrl) {}

void HttpClient::get(const std::string& endpoint) {
    std::string fullUrl = serverUrl + endpoint;
    std::cout << "GET request to " << fullUrl << std::endl;

    // Send GET request using cpr library
    cpr::Response response = cpr::Get(cpr::Url{fullUrl});

    if (response.status_code == 200) {
        std::cout << "Response: " << response.text << std::endl;
    } else {
        std::cerr << "Error: " << response.status_code << " " << response.error.message << std::endl;
    }
}

void HttpClient::post(const std::string& endpoint, const std::string& data) {
    std::string fullUrl = serverUrl + endpoint;
    std::cout << "POST request to " << fullUrl << " with data: " << data << std::endl;

    // Send POST request using cpr library
    cpr::Response response = cpr::Post(cpr::Url{fullUrl}, cpr::Body{data}, cpr::Header{{"Content-Type", "application/json"}});

    if (response.status_code == 200) {
        std::cout << "Response: " << response.text << std::endl;
    } else {
        std::cerr << "Error: " << response.status_code << " " << response.error.message << std::endl;
    }
}
