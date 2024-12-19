#include <iostream>
#include "CacheSever.h"
#include "WebSocket.h"

int main(int argc, char** argv)
{
    // HTTPS server port
    int port = 8443;
    if (argc > 1)
        port = std::atoi(argv[1]);

    // Base path for server files
    std::string base_path = "/home/kale/dev/TWSStrategyCPPServer/IntradayStrategyBuilder/third_party_libs/CppServer/";
    if (argc > 2) 
        base_path = argv[2];
        std::cout << "Base PAth: " << base_path << std::endl;

    // HTTPS server content path
    std::string www = "www/wss";
    if (argc > 3)
        www = argv[3];

    const char* twsClientIp = "192.168.12.148";
    if (argc > 4) 
        twsClientIp = argv[4];

    std::cout << "WebSocket secure server port: " << port << std::endl;
    std::cout << "Websocket secure server static content path: " << www << std::endl;

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
    std::cout << "Certificate Chain File Path: " << base_path << "tools/certificates/server.pem" << std::endl;
    context->use_certificate_chain_file(base_path + "tools/certificates/server.pem");
    context->use_private_key_file(base_path + "tools/certificates/server.pem", asio::ssl::context::pem);
    context->use_tmp_dh_file(base_path + "tools/certificates/dh4096.pem");
    context->load_verify_file(base_path + "tools/certificates/ca.crt");

    // Create a new WebSocket server
    auto server = std::make_shared<TWSStrategyServer>(service, context, twsClientIp, port);

    // Start the server
    std::cout << "Server starting...";
    server->Start();
    std::cout << "Done!" << std::endl;

    std::cout << "Press Enter to stop the server or '!' to restart the server..." << std::endl;
    std::cout << std::endl;

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