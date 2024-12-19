#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "server/ws/wss_server.h"
#include "OptionScanner.h"
#include "TwsWrapper.h"
#include "ContractDefs.h"
#include "ScannerNotificationHandler.h"
#include "SocketDataCollector.h"
#include "../generated/messages.pb.h"

#include <iostream>

// Websocket message options
class WSMessages {
    public:
        static std::string serializeBasicMessage(const std::string& msg);
        static std::string serializeConfirmationMessage(const std::string& action, const std::string& status);
        static std::string serializeIsbActionMessage(const std::string& component, 
            const std::string& action, const std::string& data="");

};

class TWSStrategySession : public CppServer::WS::WSSSession {
    public:
        using CppServer::WS::WSSSession::WSSSession;

        TWSStrategySession(const std::shared_ptr<CppServer::WS::WSSServer>& server,
            const std::shared_ptr<tWrapper>& twsClient,
            const std::shared_ptr<ScannerNotificationBus>& scannerDataHandler,
            const std::shared_ptr<SocketDataCollector> sdc,
            const std::shared_ptr<OptionScanner>& optScanner);

    protected:
        void onWSConnected(const CppServer::HTTP::HTTPRequest& request) override;
        void onWSDisconnected() override;
        void sendSerializedMessage(const std::string& serialized);
        void onWSReceived(const void* buffer, size_t size) override;
        void onWSPing(const void* buffer, size_t size) override;
        void onError(int error, const std::string& category, const std::string& message) override;

        // Receiving serialized messages
        void onIsbActionMessage(Message& message);

    private:
        std::shared_ptr<tWrapper> twsClient;
        std::shared_ptr<ScannerNotificationBus> scannerDataHandler;
        std::shared_ptr<SocketDataCollector> sdc;
        std::shared_ptr<OptionScanner> optScanner;

        std::mutex scannerMutex;
};

class TWSStrategyServer : public CppServer::WS::WSSServer {
    public:
        using CppServer::WS::WSSServer::WSSServer;

        TWSStrategyServer(const std::shared_ptr<CppServer::Asio::Service>& service,
            const std::shared_ptr<CppServer::Asio::SSLContext>& context,
            const char* twsClientIp,  int port);

    protected:
        std::shared_ptr<CppServer::Asio::SSLSession> CreateSession(const std::shared_ptr<CppServer::Asio::SSLServer>& server) override; 
        void onError(int error, const std::string& category, const std::string& message) override;

    private:
        std::shared_ptr<tWrapper> twsClient;
        std::shared_ptr<ScannerNotificationBus> scannerDataHandler;
        std::shared_ptr<SocketDataCollector> sdc = nullptr;
        std::shared_ptr<OptionScanner> optScanner = nullptr;

        const char* twsClientIp;
        int twsClientPort = 7496;

        friend class TWSStrategySession;
};

#endif