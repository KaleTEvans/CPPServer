#include "WebSocket.h"

void TWSStrategySession::onWSConnected(const CppServer::HTTP::HTTPRequest& request) {
    std::cout << "TWS Strategy Server WebSocket secure session with ID " << id() << " connected." << std::endl;

    // Send invite message
    std::string message("Pleasse send a message or '!' to disconnect.");
    SendTextAsync(message);
}

void TWSStrategySession::onWSDisconnected() {
    std::cout << "WebSocket secure session with ID " << id() << " has been disconnected." << std::endl;
}

void TWSStrategySession::onWSReceived(const void* buffer, size_t size) {
    std::string message((const char*)buffer, size);
    std::cout << "Incoming: " << message << std::endl;

    // Multicast message to all connected sessions
    std::dynamic_pointer_cast<CppServer::WS::WSSServer>(server())->MulticastText(message);

    // If the buffer starts with '!' the disconnect the current session
    if (message == "!")
        Close(1000);
}

void TWSStrategySession::onWSPing(const void* buffer, size_t size) {
    SendPongAsync(buffer, size);
}

void TWSStrategySession::onError(int error, const std::string& category, const std::string& message) {
    std::cout << "TWS Strategy Server WebSocket secure session caught an error with code " 
        << error << " and category '" << category << "': " << message << std::endl;
}

std::shared_ptr<CppServer::Asio::SSLSession> TWSStrategyServer::CreateSession(std::shared_ptr<CppServer::Asio::SSLServer> server) {
    return std::make_shared<TWSStrategySession>(std::dynamic_pointer_cast<CppServer::WS::WSSServer>(server));
}

void TWSStrategyServer::onError(int error, const std::string& category, const std::string& message) {
    std::cout << "TWS Strategy Server WebSocket secure session caught an error with code " 
        << error << " and category '" << category << "': " << message << std::endl;
}
