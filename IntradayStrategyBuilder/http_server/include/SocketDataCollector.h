// Use for collecting and formatting all desired TWS data to send through the socket

#ifndef SOCKETDATACOLLECTOR_H
#define SOCKETDATACOLLECTOR_H

#include <map>
#include <vector>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "server/ws/wss_server.h"
#include "../generated/messages.pb.h"

class SocketDataCollector {
    public:
        SocketDataCollector(const std::shared_ptr<CppServer::WS::WSSServer>& server);

        void sendNewsData(const std::string& news);

    private:
        std::shared_ptr<CppServer::WS::WSSServer> server;
        std::mutex bufferMutex;
        std::thread collectorThread;
        bool stopFlag;
};

#endif