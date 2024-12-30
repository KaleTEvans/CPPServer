#include "WebSocket.h"

std::string WSMessages::serializeBasicMessage(const std::string& msg) {
    Message message;
    message.set_type("basic_message");
    BasicMessage* basicMessage = message.mutable_basic_message();
    basicMessage->set_message(msg);

    std::string serialized;
    message.SerializeToString(&serialized);

    return serialized;
}

std::string WSMessages::serializeConfirmationMessage(const std::string& action, const std::string& status) {
    Message message;
    message.set_type("confirmation");

    Confirmation* confirmation = message.mutable_confirmation();
    confirmation->set_action(action);
    confirmation->set_status(status);
    
    std::string serialized;
    message.SerializeToString(&serialized);

    return serialized;
}

std::string WSMessages::serializeIsbActionMessage(const std::string& component, 
    const std::string& action, const std::string& data) {
    Message message;
    message.set_type("isb_action");

    ISBAction* isb_action = message.mutable_isb_action();
    isb_action->set_component(component);
    isb_action->set_action(action);
    isb_action->set_data(data);

    std::string serialized;
    message.SerializeToString(&serialized);

    return serialized;
}

TWSStrategySession::TWSStrategySession(const std::shared_ptr<CppServer::WS::WSSServer>& server,
                                       const std::shared_ptr<tWrapper>& twsClient,
                                       const std::shared_ptr<ScannerNotificationBus>& scannerDataHandler,
                                       const std::shared_ptr<SocketDataCollector> sdc,
                                       const std::shared_ptr<OptionScanner>& optScanner)
    : CppServer::WS::WSSSession(server), twsClient(twsClient), 
    scannerDataHandler(scannerDataHandler), sdc(sdc), optScanner(optScanner) {}

void TWSStrategySession::onWSConnected(const CppServer::HTTP::HTTPRequest& request) {
    std::cout << "TWS Strategy Server WebSocket secure session with ID " << id() << " connected." << std::endl;

    if (twsClient->isConnected()) {
        std::string action_msg = "connected_to_tws";
        std::string status_msg = "success";
        std::string confirmation_msg = WSMessages::serializeConfirmationMessage(action_msg, status_msg);;
        sendSerializedMessage(confirmation_msg);

        // **** TEST *****
        // std::string ticker = "SPX";
        // std::string action = "add_ticker";
        // std::string component = "option_scanner";
        // std::string isb_action_msg = WSMessages::serializeIsbActionMessage(component, action, ticker);
        // sendSerializedMessage(isb_action_msg);

        // action = "start";
        // isb_action_msg = WSMessages::serializeIsbActionMessage(component, action);
        // sendSerializedMessage(isb_action_msg);
    }

    if (!scannerDataHandler) {if (optScanner) 
        scannerDataHandler = std::make_shared<ScannerNotificationBus>();
    }

    // Send invite message
    std::string msg = "Pleasse send a message or '!' to disconnect.";
    std::string basic_msg = WSMessages::serializeBasicMessage(msg);

    sendSerializedMessage(basic_msg);
}

void TWSStrategySession::onWSDisconnected() {
    std::cout << "WebSocket secure session with ID " << id() << " has been disconnected." << std::endl;
}

void TWSStrategySession::sendSerializedMessage(const std::string& serialized) {
    try {
        std::cout << "Sending serialized message of size: " << serialized.size() << std::endl;
        SendBinaryAsync(serialized.data(), serialized.size());
    } catch (const std::exception& e) {
        std::cerr << "Error sending message: " << e.what() << std::endl;
    }
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
        // messageString = basicMessage.message();
    } else if (message.type() == "confirmation" && message.has_confirmation()) {
        const Confirmation& confirmation = message.confirmation();
        // messageString = confirmation.action() + " current status: " + confirmation.status();
    } else if (message.type() == "isb_action" && message.has_isb_action()) {
        onIsbActionMessage(message);
    }
 
    // Multicast message to all connected sessions
    // std::cout << "[Message Received] " << messageString << std::endl;
    // std::dynamic_pointer_cast<CppServer::WS::WSSServer>(server())->MulticastText(messageString);

    // If the buffer starts with '!' the disconnect tconst ISBAction& isbAction = message.isb_action();he current session
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

void TWSStrategySession::onIsbActionMessage(Message& message) {
    const ISBAction& isbAction = message.isb_action();

    if (isbAction.component() == "option_scanner") {
        std::lock_guard<std::mutex> lock(scannerMutex);
        if (!twsClient || !scannerDataHandler) return;
        if (!optScanner) optScanner = std::make_shared<OptionScanner>(twsClient, scannerDataHandler, sdc);

        if (isbAction.action() == "start") {
            optScanner->start();
            // Send confirmation
            std::string action = "start_option_scanner";
            std::string status = "success";
            std::string messageString = WSMessages::serializeConfirmationMessage(action, status);
            std::dynamic_pointer_cast<CppServer::WS::WSSServer>(server())->MulticastBinary(messageString);
        } else if (isbAction.action() == "stop") {
            optScanner->stop();
            // Send confirmation
            std::string action = "stop_option_scanner";
            std::string status = "success";
            std::string messageString = WSMessages::serializeConfirmationMessage(action, status);
            std::dynamic_pointer_cast<CppServer::WS::WSSServer>(server())->MulticastBinary(messageString);
        } else if (isbAction.action() == "add_ticker") {
            std::string ticker = isbAction.data();
            if (ticker == "SPX") {
                Contract spx = ContractDefs::SPXInd();
                Contract spxOpt = ContractDefs::SPXOpt0DTE("C", 1000);
                optScanner->addSecurity(spx, spxOpt);
                // Send confirmation
                std::string action = "add_ticker";
                std::string status = "success";
                std::string messageString = WSMessages::serializeConfirmationMessage(action, status);
                std::dynamic_pointer_cast<CppServer::WS::WSSServer>(server())->MulticastBinary(messageString);
            }
        } else if (isbAction.action() == "check_running") {
            if (optScanner->checkScannerRunning()) {
                std::string action = "check_scanner_running";
                std::string status = "yes";
                std::string messageString = WSMessages::serializeConfirmationMessage(action, status);
                std::dynamic_pointer_cast<CppServer::WS::WSSServer>(server())->MulticastBinary(messageString);
            } else {
                std::string action = "check_scanner_running";
                std::string status = "no";
                std::string messageString = WSMessages::serializeConfirmationMessage(action, status);
                std::dynamic_pointer_cast<CppServer::WS::WSSServer>(server())->MulticastBinary(messageString);
            }
        }
    }
}

TWSStrategyServer::TWSStrategyServer(const std::shared_ptr<CppServer::Asio::Service>& service,
                                     const std::shared_ptr<CppServer::Asio::SSLContext>& context,
                                     const char* twsClientIp, int port)
    : CppServer::WS::WSSServer(service, context, port), twsClientIp(twsClientIp) {
    // Server setup options
    SetupKeepAlive(true);
    SetupNoDelay(true);

    // Initialize shared objects
    twsClient = std::make_shared<tWrapper>();
    scannerDataHandler = std::make_shared<ScannerNotificationBus>();

    // Connect the TWS client
    int clientId = 0;
    twsClient->connect(twsClientIp, twsClientPort, clientId);
    twsClient->startMsgProcessingThread();

    if (twsClient->isConnected()) {
        std::cout << "TWS client connected successfully!" << std::endl;
    }
}

std::shared_ptr<CppServer::Asio::SSLSession> TWSStrategyServer::CreateSession(
    const std::shared_ptr<CppServer::Asio::SSLServer>& server) {

    if (!sdc && !optScanner) {
        sdc = std::make_shared<SocketDataCollector>(std::dynamic_pointer_cast<CppServer::WS::WSSServer>(shared_from_this()));
        optScanner = std::make_shared<OptionScanner>(twsClient, scannerDataHandler, sdc);
    }

    return std::make_shared<TWSStrategySession>(
        std::dynamic_pointer_cast<CppServer::WS::WSSServer>(server),
        twsClient,
        scannerDataHandler,
        sdc,
        optScanner);
}

void TWSStrategyServer::onError(int error, const std::string& category, const std::string& message) {
    std::cout << "TWS Strategy Server WebSocket secure session caught an error with code " 
        << error << " and category '"   << category << "': " << message << std::endl;
}
