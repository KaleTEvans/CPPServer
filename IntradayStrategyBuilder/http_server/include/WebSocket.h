#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "server/ws/wss_server.h"

#include <iostream>

class TWSStrategySession : public CppServer::WS::WSSSession {
    public:
        using CppServer::WS::WSSSession::WSSSession;

    protected:
        void onWSConnected(const CppServer::HTTP::HTTPRequest& request) override;
        void onWSDisconnected() override;
        void onWSReceived(const void* buffer, size_t size) override;
        void onWSPing(const void* buffer, size_t size) override;
        void onError(int error, const std::string& category, const std::string& message) override;
};

class TWSStrategyServer : public CppServer::WS::WSSServer {
    public:
        using CppServer::WS::WSSServer::WSSServer;

    protected:
        std::shared_ptr<CppServer::Asio::SSLSession> CreateSession(std::shared_ptr<CppServer::Asio::SSLServer> server);
        void onError(int error, const std::string& category, const std::string& message) override;
};

#endif