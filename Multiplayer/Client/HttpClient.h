#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>

class HttpClient {
public:
    HttpClient(const std::string& serverUrl);
    void get(const std::string& endpoint);
    void post(const std::string& endpoint, const std::string& data);

private:
    std::string serverUrl;
};

#endif // HTTPCLIENT_H
