#include "CacheSever.h"

std::string Cache::GetAllCache()
{
    std::scoped_lock locker(_cache_lock);
    std::string result;
    result += "[\n";
    for (const auto& item : _cache)
    {
        result += "  {\n";
        result += "    \"key\": \"" + item.first + "\",\n";
        result += "    \"value\": \"" + item.second + "\",\n";
        result += "  },\n";
    }
    result += "]\n";
    return result;
}

bool Cache::GetCacheValue(std::string_view key, std::string& value)
{
    std::scoped_lock locker(_cache_lock);
    auto it = _cache.find(key);
    if (it != _cache.end())
    {
        value = it->second;
        return true;
    }
    else
        return false;
}

void Cache::PutCacheValue(std::string_view key, std::string_view value)
{
    std::scoped_lock locker(_cache_lock);
    auto it = _cache.emplace(key, value);
    if (!it.second)
        it.first->second = value;
}

bool Cache::DeleteCacheValue(std::string_view key, std::string& value)
{
    std::scoped_lock locker(_cache_lock);
    auto it = _cache.find(key);
    if (it != _cache.end())
    {
        value = it->second;
        _cache.erase(it);
        return true;
    }
    else
        return false;
}

void HTTPSCacheSession::onReceivedRequest(const CppServer::HTTP::HTTPRequest& request)
{
    // Show HTTP request content
    std::cout << std::endl << request;

    // Process HTTP request methods
    if (request.method() == "HEAD")
        SendResponseAsync(response().MakeHeadResponse());
    else if (request.method() == "GET")
    {
        std::string key(request.url());
        std::string value;

        // Decode the key value
        key = CppCommon::Encoding::URLDecode(key);
        CppCommon::StringUtils::ReplaceFirst(key, "/api/cache", "");
        CppCommon::StringUtils::ReplaceFirst(key, "?key=", "");

        if (key.empty())
        {
            // Response with all cache values
            SendResponseAsync(response().MakeGetResponse(Cache::GetInstance().GetAllCache(), "application/json; charset=UTF-8"));
        }
        // Get the cache value by the given key
        else if (Cache::GetInstance().GetCacheValue(key, value))
        {
            // Response with the cache value
            SendResponseAsync(response().MakeGetResponse(value));
        }
        else
            SendResponseAsync(response().MakeErrorResponse(404, "Required cache value was not found for the key: " + key));
    }
    else if ((request.method() == "POST") || (request.method() == "PUT"))
    {
        std::string key(request.url());
        std::string value(request.body());

        // Decode the key value
        key = CppCommon::Encoding::URLDecode(key);
        CppCommon::StringUtils::ReplaceFirst(key, "/api/cache", "");
        CppCommon::StringUtils::ReplaceFirst(key, "?key=", "");

        // Put the cache value
        Cache::GetInstance().PutCacheValue(key, value);

        // Response with the cache value
        SendResponseAsync(response().MakeOKResponse());
    }
    else if (request.method() == "DELETE")
    {
        std::string key(request.url());
        std::string value;

        // Decode the key value
        key = CppCommon::Encoding::URLDecode(key);
        CppCommon::StringUtils::ReplaceFirst(key, "/api/cache", "");
        CppCommon::StringUtils::ReplaceFirst(key, "?key=", "");

        // Delete the cache value
        if (Cache::GetInstance().DeleteCacheValue(key, value))
        {
            // Response with the cache value
            SendResponseAsync(response().MakeGetResponse(value));
        }
        else
            SendResponseAsync(response().MakeErrorResponse(404, "Deleted cache value was not found for the key: " + key));
    }
    else if (request.method() == "OPTIONS")
        SendResponseAsync(response().MakeOptionsResponse());
    else if (request.method() == "TRACE")
        SendResponseAsync(response().MakeTraceResponse(request.cache()));
    else
        SendResponseAsync(response().MakeErrorResponse("Unsupported HTTP method: " + std::string(request.method())));
}

void HTTPSCacheSession::onReceivedRequestError(const CppServer::HTTP::HTTPRequest& request, const std::string& error)
{
    std::cout << "Request error: " << error << std::endl;
}

void HTTPSCacheSession::onError(int error, const std::string& category, const std::string& message)
{
    std::cout << "HTTPS session caught an error with code " << error << " and category '" << category << "': " << message << std::endl;
}

std::shared_ptr<CppServer::Asio::SSLSession> HTTPSCacheServer::CreateSession(const std::shared_ptr<CppServer::Asio::SSLServer>& server)
{
    return std::make_shared<HTTPSCacheSession>(std::dynamic_pointer_cast<CppServer::HTTP::HTTPSServer>(server));
}

void HTTPSCacheServer::onError(int error, const std::string& category, const std::string& message)
{
    std::cout << "HTTPS server caught an error with code " << error << " and category '" << category << "': " << message << std::endl;
}