#include "HttpClient.h"
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <winhttp.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>

#pragma comment(lib, "winhttp.lib")

HttpClient::HttpClient() {
    hSession = WinHttpOpen(L"WinHTTP Example/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        throw std::runtime_error("Failed to initialize WinHTTP session");
    }
}

HttpClient::~HttpClient() {
    if (hSession) {
        WinHttpCloseHandle(hSession);
    }
}

std::wstring HttpClient::stringToWideString(const std::string& str) {
    if (str.empty()) return std::wstring();
    
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string HttpClient::wideStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::string HttpClient::get(const std::string& url) {
    std::wstring wideUrl = stringToWideString(url);
    
    URL_COMPONENTS urlComp = { 0 };
    urlComp.dwStructSize = sizeof(urlComp);
    urlComp.dwHostNameLength = (DWORD)-1;
    urlComp.dwUrlPathLength = (DWORD)-1;
    urlComp.dwExtraInfoLength = (DWORD)-1;
    
    if (!WinHttpCrackUrl(wideUrl.c_str(), (DWORD)wideUrl.length(), 0, &urlComp)) {
        throw std::runtime_error("Failed to parse URL");
    }
    
    std::wstring hostName(urlComp.lpszHostName, urlComp.dwHostNameLength);
    std::wstring urlPath(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);
    if (urlComp.lpszExtraInfo) {
        urlPath += std::wstring(urlComp.lpszExtraInfo, urlComp.dwExtraInfoLength);
    }
    
    HINTERNET hConnect = WinHttpConnect(hSession, hostName.c_str(), urlComp.nPort, 0);
    if (!hConnect) {
        throw std::runtime_error("Failed to connect to server");
    }
    
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", urlPath.c_str(),
        NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        throw std::runtime_error("Failed to create request");
    }
    
    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        throw std::runtime_error("Failed to send request");
    }
    
    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        throw std::runtime_error("Failed to receive response");
    }
    
    std::string response;
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    std::vector<char> buffer(4096);
    
    do {
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
            break;
        }
        
        if (dwSize == 0) break;
        
        if (dwSize > buffer.size()) {
            buffer.resize(dwSize);
        }
        
        if (!WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded)) {
            break;
        }
        
        response.append(buffer.data(), dwDownloaded);
    } while (dwSize > 0);
    
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    
    return response;
}

std::string HttpClient::post(const std::string& url, const std::string& data) {
    std::wstring wideUrl = stringToWideString(url);
    std::wstring wideData = stringToWideString(data);
    
    URL_COMPONENTS urlComp = { 0 };
    urlComp.dwStructSize = sizeof(urlComp);
    urlComp.dwHostNameLength = (DWORD)-1;
    urlComp.dwUrlPathLength = (DWORD)-1;
    urlComp.dwExtraInfoLength = (DWORD)-1;
    
    if (!WinHttpCrackUrl(wideUrl.c_str(), (DWORD)wideUrl.length(), 0, &urlComp)) {
        throw std::runtime_error("Failed to parse URL");
    }
    
    std::wstring hostName(urlComp.lpszHostName, urlComp.dwHostNameLength);
    std::wstring urlPath(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);
    if (urlComp.lpszExtraInfo) {
        urlPath += std::wstring(urlComp.lpszExtraInfo, urlComp.dwExtraInfoLength);
    }
    
    HINTERNET hConnect = WinHttpConnect(hSession, hostName.c_str(), urlComp.nPort, 0);
    if (!hConnect) {
        throw std::runtime_error("Failed to connect to server");
    }
    
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", urlPath.c_str(),
        NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        throw std::runtime_error("Failed to create request");
    }
    
    std::wstring headers = L"Content-Type: application/json\r\n";
    if (!WinHttpAddRequestHeaders(hRequest, headers.c_str(), (DWORD)headers.length(), WINHTTP_ADDREQ_FLAG_ADD)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        throw std::runtime_error("Failed to add headers");
    }
    
    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        (LPVOID)wideData.c_str(), (DWORD)wideData.length(), (DWORD)wideData.length(), 0)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        throw std::runtime_error("Failed to send request");
    }
    
    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        throw std::runtime_error("Failed to receive response");
    }
    
    std::string response;
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    std::vector<char> buffer(4096);
    
    do {
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
            break;
        }
        
        if (dwSize == 0) break;
        
        if (dwSize > buffer.size()) {
            buffer.resize(dwSize);
        }
        
        if (!WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded)) {
            break;
        }
        
        response.append(buffer.data(), dwDownloaded);
    } while (dwSize > 0);
    
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    
    return response;
} 