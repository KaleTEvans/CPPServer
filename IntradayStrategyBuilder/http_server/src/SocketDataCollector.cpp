#include "SocketDataCollector.h"

SocketDataCollector::SocketDataCollector(const std::shared_ptr<CppServer::WS::WSSServer>& server) :
    server(server), stopFlag(false) {}

void SocketDataCollector::sendOptionData(const std::string& od) {
    server->MulticastBinary(od.data(), od.size());
}

void SocketDataCollector::sendNewsData(const std::string& news) {
    server->MulticastBinary(news.data(), news.size());
}

void SocketDataCollector::sendUnderlyingContractData(const std::string& cd) {
    server->MulticastBinary(cd.data(), cd.size());
}