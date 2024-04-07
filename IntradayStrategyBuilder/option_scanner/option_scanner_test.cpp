#include <iostream>
#include <set>

// #include "TwsWrapper.h"

// // to use the Sleep function
// #ifdef WIN32
// 	#include <windows.h>		// Sleep(), in miliseconds
// 	#include <process.h>
// 	#define CurrentThreadId GetCurrentThreadId
// #else
// 	#include <unistd.h>			// usleep(), in microseconds
// 	#define Sleep( m ) usleep( m*1000 )
// 	#include <pthread.h>
// 	#define CurrentThreadId pthread_self
// #endif

// #define PrintProcessId printf("%ld  ", CurrentThreadId() )

// class Client {
//     public:
//         Client(std::shared_ptr<MessageBus> bus) {
//             bus->subscribe(EventType::ContractInfo, [this](std::shared_ptr<DataEvent> event) {
//                 this->handleContractDataEvent(std::dynamic_pointer_cast<ContractDataEvent>(event));
//             });

//             bus->subscribe(EventType::TickPriceInfo, [this](std::shared_ptr<DataEvent> event) {
//                 this->handleTickPriceEvent(std::dynamic_pointer_cast<TickPriceEvent>(event));
//             });

//             bus->subscribe(EventType::TickNewsInfo, [this](std::shared_ptr<DataEvent> event) {
//                 this->handleTickNewsEvent(std::dynamic_pointer_cast<TickNewsEvent>(event));
//             });

//             bus->subscribe(EventType::EndOfRequest, [this](std::shared_ptr<DataEvent> event) {
//                 this->handleDataEvent(std::dynamic_pointer_cast<EndOfRequestEvent>(event));
//             });
//         }

//         void handleContractDataEvent(std::shared_ptr<ContractDataEvent> event) {
//             const int reqId = event->reqId;
//             if (reqIdList.find(reqId) != reqIdList.end()) {
//                 const ContractDetails& cd = event->details;
//                 container.push_back(cd);
//             }
//         }

//         void handleTickPriceEvent(std::shared_ptr<TickPriceEvent> event) {
//             const int reqId = event->reqId;
//             if (reqIdList.find(reqId) != reqIdList.end()) {
//                 std::cout << "Current Price: " << event->price << std::endl;
//                 price = event->price;
//             }
//         }

//         void handleTickNewsEvent(std::shared_ptr<TickNewsEvent> event) {
//             const int reqId = event->reqId;
//             if (reqIdList.find(reqId) != reqIdList.end()) {
//                 std::cout << "News ID: " << event->newsId << std::endl;
//                 std::cout << "Time of Article: " << event->dateTime << std::endl;
//                 std::cout << "Headline: " << event->headline << std::endl;
//             }
//         }

//         void handleDataEvent(std::shared_ptr<EndOfRequestEvent> event) {
//             const int reqId = event->reqId;
//             if (reqIdList.find(reqId) != reqIdList.end()) {
//                 dataReceived = true;
//                 reqIdList.erase(reqId);
//             }

//             std::cout << "End of request for ID: " << reqId << std::endl;
//         }

//         void addReqId(int reqId) { reqIdList.insert(reqId); }
//         std::vector<ContractDetails> getContainer() { return container; }

//         bool dataReceived{ false }; 
//         double price{0};

//     private:
//         std::set<TickerId> reqIdList;
//         std::vector<ContractDetails> container;
        
// };

// struct Contract_ : public Contract
// {
// 	Contract_( IBString sb, IBString st, IBString cr, IBString ex, IBString pr_ex, IBString exp )
// 	: Contract()
// 	{
// 		symbol				= sb;
// 		secType				= st;		//"STK"
// 		currency			= cr;
// 		exchange			= ex;	  	//"SMART";
// 		primaryExchange 	= pr_ex;	//"ISLAND";
//         expiry              = exp;
// 	}
// };

int main() {
    return 0;
}

// int main(void) {
//     Contract_			CC( "NVDA", *SecType::STK, "USD", *Exchange::IB_SMART, *Exchange::ISLAND, EndDateTime(2024, 04, 05) );
//     Contract_			CD( "NVDA", *SecType::OPT, "USD", *Exchange::IB_SMART, *Exchange::ISLAND, EndDateTime(2024, 04, 05) );

//     tWrapper twsWrapper;
//     EClientL0*	EC = EClientL0::New( &twsWrapper );
//     Client client(twsWrapper.getMessageBus());

//     printf( "ClientVersion = %d\n", EC->clientVersion() );

//     // Connect to tws 
//     EC->eConnect( "192.168.12.148", 7496, 100 );

//     EC->reqCurrentTime();

//     // Get latest price data
//     client.addReqId(1201);
//     EC->reqMktData(1201, CC, "", true);

//     client.addReqId(1234);
//     EC->reqContractDetails(1234, CD);
//     std::cout << "Sent contract request" << std::endl;

//     while (!client.dataReceived) {
//         if (client.dataReceived == true) break;
//         continue;
//     }

//     for (auto con : client.getContainer()) {
//         std::cout << con.summary.localSymbol << " " << con.summary.expiry << " " << 
//         con.summary.strike << " " << con.summary.right << std::endl;
//     }

//     Contract con;
//     con.symbol = "BZ:BZ_ALL";
//     con.secType = "NEWS";
//     con.exchange = "BZ";

//     client.addReqId(1001);
//     EC->reqMktData(1001, con, "mdoff,292", false);

//     while (true) Sleep(100);

//     return 0;
// }