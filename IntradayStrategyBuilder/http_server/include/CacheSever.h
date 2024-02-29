// Will respond with program data that is meant to be kept temporary while program is running
// For example, 5 second time frame candles

#ifndef CACHESERVER_H
#define CACHESERVER_H

#include "server/http/https_server.h"
#include "string/string_utils.h"
#include "utility/singleton.h"

#include <iostream>
#include <map>
#include <mutex>

class Cache : public CppCommon::Singleton<Cache>
{
   friend CppCommon::Singleton<Cache>;

public:
    std::string GetAllCache();
    bool GetCacheValue(std::string_view key, std::string& value);
    void PutCacheValue(std::string_view key, std::string_view value);
    bool DeleteCacheValue(std::string_view key, std::string& value);

private:
    std::mutex _cache_lock;
    std::map<std::string, std::string, std::less<>> _cache;
};

class HTTPSCacheSession : public CppServer::HTTP::HTTPSSession
{
public:
    using CppServer::HTTP::HTTPSSession::HTTPSSession;

protected:
    void onReceivedRequest(const CppServer::HTTP::HTTPRequest& request) override;
    void onReceivedRequestError(const CppServer::HTTP::HTTPRequest& request, const std::string& error) override;
    void onError(int error, const std::string& category, const std::string& message) override;
};

class HTTPSCacheServer : public CppServer::HTTP::HTTPSServer
{
public:
    using CppServer::HTTP::HTTPSServer::HTTPSServer;

protected:
    std::shared_ptr<CppServer::Asio::SSLSession> CreateSession(const std::shared_ptr<CppServer::Asio::SSLServer>& server) override;

protected:
    void onError(int error, const std::string& category, const std::string& message) override;
};

#endif