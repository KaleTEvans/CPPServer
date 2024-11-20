#include "WebSocket.h"
#include "../generated/messages.pb.h"

void TWSStrategySession::onWSConnected(const CppServer::HTTP::HTTPRequest& request) {
    std::cout << "TWS Strategy Server WebSocket secure session with ID " << id() << " connected." << std::endl;

    if (!twsClient) {
        twsClient = std::make_shared<tWrapper>();
        int clientId = 0;
        twsClient->connect(twsClientIp, 7496, clientId);
        twsClient->startMsgProcessingThread();

        if (twsClient->isConnected()) {
            std::cout << "TWS client connected successfully!" << std::endl;
            Message message;
            message.set_type("confirmation");
            Confirmation* confirmation = message.mutable_confirmation();
            confirmation->set_action("connected_to_tws");
            confirmation->set_status("success");
            
            std::string serialized;
            message.SerializeToString(&serialized);

            sendSerializedMessage(serialized);
        }
    }

    // Send invite message
    Message message;
    message.set_type("basic_message");
    BasicMessage* basicMessage = message.mutable_basic_message();
    basicMessage->set_message("Pleasse send a message or '!' to disconnect.");

    std::string serialized;
    message.SerializeToString(&serialized);

    sendSerializedMessage(serialized);
}

void TWSStrategySession::onWSDisconnected() {
    std::cout << "WebSocket secure session with ID " << id() << " has been disconnected." << std::endl;
}

void TWSStrategySession::sendSerializedMessage(const std::string& serialized) {
    std::cout << "Sending serialized message of size: " << serialized.size() << std::endl;
    SendBinaryAsync(serialized.data(), serialized.size());

    // Simulate the server receiving its own messages for debugging purposes
    onWSReceived(serialized.data(), serialized.size());
}

void TWSStrategySession::onWSReceived(const void* buffer, size_t size) {
    std::cout << "Received message of size: " << size << std::endl;
    // Deserialize the protobuff message
    Message message;
    if (!message.ParseFromArray(buffer, size)) {
        std::cerr << "Failed to parse Protobuf message!" << std::endl;
        return;
    }

    std::string messageString = "";

    if (message.type() == "basic_message" && message.has_basic_message()) {
        const BasicMessage& basicMessage = message.basic_message();
        messageString = basicMessage.message();
    } else if (message.type() == "confirmation" && message.has_confirmation()) {
        const Confirmation& confirmation = message.confirmation();
        messageString = confirmation.action() + " current status: " + confirmation.status();
    } else if (message.type() == "isb_action" && message.has_isb_action()) {
        const ISBAction& isbAction = message.isb_action();
    }
 
    // Multicast message to all connected sessions
    std::cout << "Message to multicast: " << messageString << std::endl;
    std::dynamic_pointer_cast<CppServer::WS::WSSServer>(server())->MulticastText(messageString);

    // If the buffer starts with '!' the disconnect the current session
    if (messageString == "!")
        Close(1000);
}

void TWSStrategySession::onWSPing(const void* buffer, size_t size) {
    SendPongAsync(buffer, size);
}

void TWSStrategySession::onError(int error, const std::string& category, const std::string& message) {
    std::cout << "TWS Strategy Server WebSocket secure session caught an error with code " 
        << error << " and category '" << category << "': " << message << std::endl;
}

std::shared_ptr<CppServer::Asio::SSLSession> TWSStrategyServer::CreateSession(
    const std::shared_ptr<CppServer::Asio::SSLServer>& server) {
    std::cout << "Creating new WebSocket session..." << std::endl;
    return std::make_shared<TWSStrategySession>(
        std::dynamic_pointer_cast<CppServer::WS::WSSServer>(server));
}

void TWSStrategyServer::onError(int error, const std::string& category, const std::string& message) {
    std::cout << "TWS Strategy Server WebSocket secure session caught an error with code " 
        << error << " and category '"   << category << "': " << message << std::endl;
}
