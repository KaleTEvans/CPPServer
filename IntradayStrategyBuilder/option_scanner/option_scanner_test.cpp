#include <iostream>
#include <set>

#include "TwsWrapper.h"

// to use the Sleep function
#ifdef WIN32
	#include <windows.h>		// Sleep(), in miliseconds
	#include <process.h>
	#define CurrentThreadId GetCurrentThreadId
#else
	#include <unistd.h>			// usleep(), in microseconds
	#define Sleep( m ) usleep( m*1000 )
	#include <pthread.h>
	#define CurrentThreadId pthread_self
#endif

#define PrintProcessId printf("%ld  ", CurrentThreadId() )

class Client {
    public:
        Client(std::shared_ptr<MessageBus> bus) {
            bus->subscribe(EventType::ContractInfo, [this](std::shared_ptr<DataEvent> event) {
                this->handleContractDataEvent(std::dynamic_pointer_cast<ContractDataEvent>(event));
            });

            bus->subscribe(EventType::EndOfRequest, [this](std::shared_ptr<DataEvent> event) {
                this->handleDataEvent(std::dynamic_pointer_cast<EndOfRequestEvent>(event));
            });
        }

        void handleContractDataEvent(std::shared_ptr<ContractDataEvent> event) {
            const int reqId = event->reqId;
            std::cout << "Request received" << std::endl;
            if (reqIdList.find(reqId) != reqIdList.end()) {
                const ContractDetails& cd = event->details;
                container.push_back(cd);
            }
        }

        void handleDataEvent(std::shared_ptr<EndOfRequestEvent> event) {
            const int reqId = event->reqId;
            if (reqIdList.find(reqId) != reqIdList.end()) {
                dataReceived = true;
                reqIdList.erase(reqId);
            }

            std::cout << "End of request for ID: " << reqId << std::endl;
        }

        void addReqId(int reqId) { reqIdList.insert(reqId); }
        std::vector<ContractDetails> getContainer() { return container; }

        bool dataReceived{ false }; 

    private:
        std::set<TickerId> reqIdList;
        std::vector<ContractDetails> container;
};

struct Contract_ : public Contract
{
	Contract_( IBString sb, IBString st, IBString cr, IBString ex, IBString pr_ex )
	: Contract()
	{
		symbol				= sb;
		secType				= st;		//"STK"
		currency			= cr;
		exchange			= ex;	  	//"SMART";
		primaryExchange 	= pr_ex;	//"ISLAND";
	}
};

int main(void) {
    Contract_			C( "NVDA", *SecType::STK, "USD", *Exchange::IB_SMART, *Exchange::ISLAND );

    tWrapper twsWrapper;
    EClientL0*	EC = EClientL0::New( &twsWrapper );
    Client client(twsWrapper.getMessageBus());

    printf( "ClientVersion = %d\n", EC->clientVersion() );

    // Connect to tws 
    EC->eConnect( "192.168.12.148", 7496, 100 );

    Sleep(100);

    EC->reqCurrentTime();

    client.addReqId(1234);
    EC->reqContractDetails(1234, C);
    std::cout << "Sent contract request" << std::endl;

    while (!client.dataReceived) {
        if (client.dataReceived == true) break;
        continue;
    }

    for (auto con : client.getContainer()) {
        std::cout << con.summary.localSymbol << " " << con.summary.expiry << " " << con.summary.strike << std::endl;
    }

    return 0;
}