#include <iostream>
#include "CacheSever.h"
#include "WebSocket.h"

int main(int argc, char** argv)
{
    // HTTPS server port
    int port = 8443;
    if (argc > 1)
        port = std::atoi(argv[1]);
    // HTTPS server content path
    std::string www = "/home/kale/dev/TWSStrategyCPPServer/IntradayStrategyBuilder/third_party_libs/CppServer/www/wss";
    if (argc > 2)
        www = argv[2];

    std::cout << "WebSocket secure server port: " << port << std::endl;
    std::cout << "Websocket secure server static content path: " << www << std::endl;
    std::cout << "Websocket secure server website: " << "https://localhost:" << port << "/chat/index.html" << std::endl;

    std::cout << std::endl;

    // Create a new Asio service
    auto service = std::make_shared<CppServer::Asio::Service>();

    // Start the Asio service
    std::cout << "Asio service starting...";
    service->Start();
    std::cout << "Done!" << std::endl;

    // Create and prepare a new SSL server context
    auto context = std::make_shared<CppServer::Asio::SSLContext>(asio::ssl::context::tlsv12);
    context->set_password_callback([](size_t max_length, asio::ssl::context::password_purpose purpose) -> std::string { return "qwerty"; });
    context->use_certificate_chain_file("/home/kale/dev/TWSStrategyCPPServer/IntradayStrategyBuilder/third_party_libs/CppServer/tools/certificates/server.pem");
    context->use_private_key_file("/home/kale/dev/TWSStrategyCPPServer/IntradayStrategyBuilder/third_party_libs/CppServer/tools/certificates/server.pem", asio::ssl::context::pem);
    context->use_tmp_dh_file("/home/kale/dev/TWSStrategyCPPServer/IntradayStrategyBuilder/third_party_libs/CppServer/tools/certificates/dh4096.pem");

    // Create a new WebSocket server
    auto server = std::make_shared<TWSStrategyServer>(service, context, port);
    server->AddStaticContent(www, "/chat");

    // Start the server
    std::cout << "Server starting...";
    server->Start();
    std::cout << "Done!" << std::endl;

    std::cout << "Press Enter to stop the server or '!' to restart the server..." << std::endl;

    // Perform text input
    std::string line;
    while (getline(std::cin, line))
    {
        if (line.empty())
            break;

        // Restart the server
        if (line == "!")
        {
            std::cout << "Server restarting...";
            server->Restart();
            std::cout << "Done!" << std::endl;
            continue;
        }
    }

    // Stop the server
    std::cout << "Server stopping...";
    server->Stop();
    std::cout << "Done!" << std::endl;

    // Stop the Asio service
    std::cout << "Asio service stopping...";
    service->Stop();
    std::cout << "Done!" << std::endl;

    return 0;
}