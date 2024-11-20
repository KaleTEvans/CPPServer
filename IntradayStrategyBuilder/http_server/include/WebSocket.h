#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "server/ws/wss_server.h"
#include "OptionScanner.h"
#include "TwsWrapper.h"
#include "ScannerNotificationHandler.h"

#include <iostream>

class TWSStrategySession : public CppServer::WS::WSSSession {
    public:
        using CppServer::WS::WSSSession::WSSSession;

    protected:
        void onWSConnected(const CppServer::HTTP::HTTPRequest& request) override;
        void onWSDisconnected() override;
        void sendSerializedMessage(const std::string& serialized);
        void onWSReceived(const void* buffer, size_t size) override;
        void onWSPing(const void* buffer, size_t size) override;
        void onError(int error, const std::string& category, const std::string& message) override;

    private:
        std::shared_ptr<tWrapper> twsClient = nullptr;
        std::shared_ptr<ScannerNotificationBus> scannerDataHandler = nullptr;
        std::shared_ptr<OptionScanner> optScanner = nullptr;

        const char* twsClientIp = "192.168.12.148";
};

class TWSStrategyServer : public CppServer::WS::WSSServer {
    public:
        using CppServer::WS::WSSServer::WSSServer;

    protected:
        std::shared_ptr<CppServer::Asio::SSLSession> CreateSession(const std::shared_ptr<CppServer::Asio::SSLServer>& server) override; 
        void onError(int error, const std::string& category, const std::string& message) override;
};

#endif