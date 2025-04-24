#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <winhttp.h>
#include <string>

class HttpClient {
public:
    HttpClient();
    ~HttpClient();
    
    std::string get(const std::string& url);
    std::string post(const std::string& url, const std::string& data);

private:
    HINTERNET hSession;
    
    std::wstring stringToWideString(const std::string& str);
    std::string wideStringToString(const std::wstring& wstr);
};

#endif // HTTPCLIENT_H
