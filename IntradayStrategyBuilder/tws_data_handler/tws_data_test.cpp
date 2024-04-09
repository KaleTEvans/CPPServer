#include <iostream>
#include <thread>
#include <chrono>

#include "TwsWrapper.h"

const unsigned MAX_ATTEMPTS = 50;
const unsigned SLEEP_TIME = 10;

class Subscriber {
public:
    Subscriber(std::shared_ptr<MessageBus> bus) {
        bus->subscribe(EventType::ContractInfo, [this](std::shared_ptr<DataEvent> event) {
            this->handleContractDataEvent(std::dynamic_pointer_cast<ContractDataEvent>(event));
        });

        bus->subscribe(EventType::TickPriceInfo, [this](std::shared_ptr<DataEvent> event) {
            this->handleTickPriceEvent(std::dynamic_pointer_cast<TickPriceEvent>(event));
        });

        bus->subscribe(EventType::TickNewsInfo, [this](std::shared_ptr<DataEvent> event) {
            this->handleTickNewsEvent(std::dynamic_pointer_cast<TickNewsEvent>(event));
        });

        bus->subscribe(EventType::EndOfRequest, [this](std::shared_ptr<DataEvent> event) {
            this->handleDataEvent(std::dynamic_pointer_cast<EndOfRequestEvent>(event));
        });
    }

    void handleContractDataEvent(std::shared_ptr<ContractDataEvent> event) {
        const int reqId = event->reqId;
        if (reqIdList.find(reqId) != reqIdList.end()) {
            std::cout << "Contract Details Received" << std::endl;
            std::cout << event->details.tradingHours << std::endl;
            const ContractDetails& cd = event->details;
            container.push_back(cd);
        }
    }

    void handleTickPriceEvent(std::shared_ptr<TickPriceEvent> event) {
        const int reqId = event->reqId;
        if (reqIdList.find(reqId) != reqIdList.end()) {
            std::cout << "Current Price: " << event->price << std::endl;
            price = event->price;
        }
    }

    void handleTickNewsEvent(std::shared_ptr<TickNewsEvent> event) {
        const int reqId = event->reqId;
        if (reqIdList.find(reqId) != reqIdList.end()) {
            std::cout << "News ID: " << event->articleId << std::endl;
            std::cout << "Time of Article: " << event->dateTime << std::endl;
            std::cout << "Headline: " << event->headline << std::endl;
        }
    }

    void handleDataEvent(std::shared_ptr<EndOfRequestEvent> event) {
        const int reqId = event->reqId;
        if (reqIdList.find(reqId) != reqIdList.end()) {
            reqIdList.erase(reqId);
            completedReqs.insert(reqId);
        }

        std::cout << "End of request for ID: " << reqId << std::endl;
    }

    void addReqId(int reqId) { reqIdList.insert(reqId); }
    bool checkCompletedReq(int reqId) { return completedReqs.find(reqId) != completedReqs.end(); }
    std::vector<ContractDetails> getContainer() { return container; }

    double price{0};

private:
    std::set<TickerId> reqIdList;
    std::set<TickerId> completedReqs;
    std::vector<ContractDetails> container;
        
};

int main() {
    tWrapper testClient;

    int clientId = 0;

	unsigned attempt = 0;
	printf( "Start of C++ Socket Client Test %u\n", attempt);

    testClient.connect("192.168.12.148", 7496, clientId);

    Subscriber testSubscriber(testClient.getMessageBus());

    // Create contract
    Contract con;
    con.symbol = "SPX";
    con.secType = "IND";
    con.currency = "USD";
    con.primaryExchange = "CBOE";

    testClient.startMsgProcessingThread();

    // Use reqContractDetails to get the contract ID
    // First add a req id to the list
    testSubscriber.addReqId(11);
    testClient.reqContractDetails(11, con);

    while (!testSubscriber.checkCompletedReq(11)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::vector<ContractDetails> cd = testSubscriber.getContainer();
    for (auto& c : cd) {
        std::cout << "Con ID: " << c.contract.conId << std::endl;
    }

    long conId = cd[0].contract.conId;
    std::string conSymbol = cd[0].contract.symbol;
    std::string secType = cd[0].contract.secType;
    std::string exchange = cd[0].contract.primaryExchange;

    testSubscriber.addReqId(12);
    testClient.reqSecDefOptParams(12, conSymbol, exchange, secType, conId);

    while (!testSubscriber.checkCompletedReq(12)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    testClient.disconnect();

    return 0;
}