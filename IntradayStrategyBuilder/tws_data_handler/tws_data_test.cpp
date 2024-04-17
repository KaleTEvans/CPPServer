#include <iostream>
#include <thread>
#include <chrono>

#include "TwsWrapper.h"
#include "TwsApiDefs.h"

using namespace TwsApi;

const unsigned MAX_ATTEMPTS = 50;
const unsigned SLEEP_TIME = 10;

class Subscriber {
public:
    Subscriber(std::shared_ptr<MessageBus> bus, tWrapper& wrapper) : wrapper(wrapper) {
        bus->subscribe(EventType::TickPriceInfo, [this](std::shared_ptr<DataEvent> event) {
            this->handleTickPriceEvent(std::dynamic_pointer_cast<TickPriceEvent>(event));
        });

        bus->subscribe(EventType::TickNewsInfo, [this](std::shared_ptr<DataEvent> event) {
            this->handleTickNewsEvent(std::dynamic_pointer_cast<TickNewsEvent>(event));
        });

        bus->subscribe(EventType::RealTimeCandleData, [this](std::shared_ptr<DataEvent> event) {
            this->realTimeCandles(std::dynamic_pointer_cast<CandleDataEvent>(event));
        });

        bus->subscribe(EventType::TickOptionInfo, [this](std::shared_ptr<DataEvent> event) {
            this->tickOptionInfo(std::dynamic_pointer_cast<TickOptionComputationEvent>(event));
        });

        bus->subscribe(EventType::TickGenericInfo, [this](std::shared_ptr<DataEvent> event) {
            this->handleTickGenericEvent(std::dynamic_pointer_cast<TickGenericEvent>(event));
        });

        bus->subscribe(EventType::TickSizeInfo, [this](std::shared_ptr<DataEvent> event) {
            this->handleTickSizeEvent(std::dynamic_pointer_cast<TickSizeEvent>(event));
        });
    }

    int getContractData(const Contract& con) {
        int reqId = 0;
        wrapper.reqContractDetails(
            [this](int reqId) {
                reqId = reqId;
            },
            [this](const ContractDetails& contractDetails) {
            this->handleContractDataEvent(contractDetails);
        }, con);
        return reqId;
    }

    void handleContractDataEvent(const ContractDetails& contractDetails) {
        container.push_back(contractDetails);
    }

    void getTime() {
        wrapper.reqCurrentTime([this](long time){
            this->printTime(time);
        });
    }

    void printTime(long time) {
        std::cout << "Time: " << time << std::endl;
    }

    void handleTickPriceEvent(std::shared_ptr<TickPriceEvent> event) {
        printf( "Tick Price. Ticker Id: %d, Field: %d, Price: %f, CanAutoExecute: %d, PastLimit: %d, PreOpen: %d\n", 
        event->reqId, (int)event->tickType, event->price, event->attrib.canAutoExecute, event->attrib.pastLimit, event->attrib.preOpen);
    }

    void handleTickGenericEvent(std::shared_ptr<TickGenericEvent> event) {
        printf( "Tick Generic. Ticker Id: %d, Type: %d, Value: %f\n", 
            event->reqId, (int)event->tickType, event->value);
    }

    void handleTickSizeEvent(std::shared_ptr<TickSizeEvent> event) {
        printf( "Tick Size. Ticker Id: %d, Field: %d, Size: %s\n", 
            event->reqId, (int)event->tickType, decimalStringToDisplay(event->size).c_str());
    }

    void handleTickNewsEvent(std::shared_ptr<TickNewsEvent> event) {
        std::cout << "News ID: " << event->articleId << std::endl;
        std::cout << "Time of Article: " << event->dateTime << std::endl;
        std::cout << "Headline: " << event->headline << std::endl;
        std::cout << "Extra Data: " << event->extraData << std::endl;
    }

    void realTimeCandles(std::shared_ptr<CandleDataEvent> event) {
        event->candle->printCandle();
    }

    void tickOptionInfo(std::shared_ptr<TickOptionComputationEvent> event) {
        printf("TickOptionComputation. Ticker Id: %d, Type: %d, TickAttrib: %d," 
        "ImpliedVolatility: %f, Delta: %f, OptionPrice: %f, pvDividend: %f, Gamma: %f," 
        "Vega: %f, Theta: %f, Underlying Price: %f\n", event->reqId, (int)event->tickType, 
        event->tickAttrib, event->impliedVol, event->delta, event->optPrice, event->pvDividend, 
        event->gamma, event->vega, event->theta, event->undPrice);
    }

    tWrapper& wrapper;

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

    Subscriber testSubscriber(testClient.getMessageBus(), testClient);

    // Create contract
    Contract con;
    con.symbol = "SPX";
    con.secType = "IND";
    con.currency = "USD";
    con.exchange = "SMART";
    con.primaryExchange = "CBOE";

    testClient.startMsgProcessingThread();

    // Use reqContractDetails to get the contract ID
    // First add a req id to the list
    int req = testSubscriber.getContractData(con);

    while (!testClient.checkEventCompleted(req)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::vector<ContractDetails> cd = testSubscriber.getContainer();
    for (auto& c : cd) {
        std::cout << "Con ID: " << c.contract.conId << std::endl;
        std::cout << c.contract.description << std::endl;
    }

    long conId = cd[0].contract.conId;
    std::string conSymbol = cd[0].contract.symbol;
    std::string secType = cd[0].contract.secType;
    std::string exchange = cd[0].contract.primaryExchange;

    Contract con1;
    con1.symbol = "SPX";
    con1.secType = "OPT";
    con1.currency = "USD";
    con1.exchange = "SMART";
    con1.primaryExchange = "CBOE";
    con1.lastTradeDateOrContractMonth = "20240412";
    con1.strike = 5110;

    Contract con2 = con1;
    con1.right = "C";
    con2.right = "P";

    //testClient.reqRealTimeBars(con1, 5, "TRADES", true);
    //testClient.reqRealTimeBars(con2, 5, "TRADES", true);
    //testClient.reqMktData(con, "100,101,106,104,225,233,293,295,411", false, false);
    
    Contract news;
    news.symbol = "BZ:BZ_ALL";
    news.secType = "NEWS";
    news.exchange = "BZ";
    
    testClient.reqMktData(news, "mdoff,292", false, false);

    for (int i=0; i < 150; i++) {
        //testSubscriber.getTime();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Cancelling data for call" << std::endl;
    //testClient.cancelRealTimeBars(5111);
    for (int i=0; i < 5000; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    testClient.disconnect();

    return 0;
}

/*
Request output for Option Contract
TickOptionComputation. Ticker Id: 101, Type: 10, TickAttrib: 0,ImpliedVolatility: 0.348064, Delta: 0.476541, OptionPrice: 5.900000, pvDividend: 0.000000, Gamma: 0.024399,Vega: 0.186759, Theta: -5.900000, Underlying Price: 5108.990000
TickOptionComputation. Ticker Id: 101, Type: 11, TickAttrib: 0,ImpliedVolatility: 0.353414, Delta: 0.476915, OptionPrice: 6.100000, pvDividend: 0.000000, Gamma: 0.024031,Vega: 0.186770, Theta: -6.100000, Underlying Price: 5108.990000
TickOptionComputation. Ticker Id: 101, Type: 12, TickAttrib: 0,ImpliedVolatility: 0.364112, Delta: 0.477631, OptionPrice: 6.000000, pvDividend: 0.000000, Gamma: 0.023328,Vega: 0.186789, Theta: -6.000000, Underlying Price: 5108.990000
TickOptionComputation. Ticker Id: 101, Type: 13, TickAttrib: 0,ImpliedVolatility: 0.350800, Delta: 0.475281, OptionPrice: 6.053478, pvDividend: 0.000000, Gamma: 0.024205,Vega: 0.186721, Theta: -6.053478, Underlying Price: 5108.930000

Request output for SPX contract
Tick Size. Ticker Id: 101, Field: 5, Size: 
Tick Size. Ticker Id: 101, Field: 5, Size: 
Tick Size. Ticker Id: 101, Field: 0, Size: 
Tick Size. Ticker Id: 101, Field: 3, Size: 
Tick Size. Ticker Id: 101, Field: 29, Size: 3949864
Tick Size. Ticker Id: 101, Field: 30, Size: 5314554
Tick Size. Ticker Id: 101, Field: 29, Size: 3949958
Tick Size. Ticker Id: 101, Field: 30, Size: 5314772

Tick Generic. Ticker Id: 101, Type: 23, Value: 0.106777
Tick Generic. Ticker Id: 101, Type: 24, Value: 0.144352
Tick Size. Ticker Id: 101, Field: 29, Size: 4063246
Tick Size. Ticker Id: 101, Field: 30, Size: 5435346
Tick Generic. Ticker Id: 101, Type: 23, Value: 0.106775
Tick Generic. Ticker Id: 101, Type: 23, Value: 0.106803

Tick Size. Ticker Id: 101, Field: 29, Size: 4069834
Tick Size. Ticker Id: 101, Field: 30, Size: 5443470
Tick Size. Ticker Id: 101, Field: 27, Size: 18698086
Tick Size. Ticker Id: 101, Field: 28, Size: 37737470

Tick Price. Ticker Id: 101, Field: 4, Price: 5120.430000, CanAutoExecute: 0, PastLimit: 0, PreOpen: 0
Tick Size. Ticker Id: 101, Field: 5, Size: 
Tick Price. Ticker Id: 101, Field: 4, Price: 5120.500000, CanAutoExecute: 0, PastLimit: 0, PreOpen: 0
Tick Size. Ticker Id: 101, Field: 5, Size: 
Tick Price. Ticker Id: 101, Field: 1, Price: 0.000000, CanAutoExecute: 0, PastLimit: 0, PreOpen: 0
Tick Size. Ticker Id: 101, Field: 0, Size: 
Tick Price. Ticker Id: 101, Field: 2, Price: 0.000000, CanAutoExecute: 0, PastLimit: 0, PreOpen: 0
Tick Size. Ticker Id: 101, Field: 3, Size: 



News ID: BZ$1729eb9e
Time of Article: 1712952261000
Headline: On April 9, Leafly Holdings Received Notice From Nasdaq That Co No Longer Complies With Nasdaq's Requirements Contained In Nasdaq Listing Rule 5550
Extra DataA:800015:L:en:K:-0.81:C:0.8131997585296631

News ID: BZ$1729eba1
Time of Article: 1712952273000
Headline: Biden Expects Iran To Attack Israel 'Sooner Rather Than Later' &#x2014; IDF: 'We'll Know How To Deal With It'
Extra DataA:800015:L:en:K:n/a:C:0.8924752473831177

News ID: BZ$1729ec32
Time of Article: 1712952278000
Headline: On April 8, Aclarion Inc Received Notice Of Delisting Or Failure To Satisfy A Continued Listing Rule Or Standard From Nasdaq
Extra DataA:800015:L:en:K:-0.99:C:0.9896387457847595

News ID: BZ$1729ecdf
Time of Article: 1712952306000
Headline: Rail Vision Files For Mixed Shelf Offering Of Up To $100M
Extra DataA:800015:L:en:K:-0.99:C:0.9919460415840149

News ID: BZ$1729f04d
Time of Article: 1712952425000
Headline: Medical Properties Trust Sells Majority Interest In Utah Hospitals;  MPT Has Retained An ~25% Interest In The Venture And The Fund Purchased An ~75% Interest For $886M, Fully Validating MPT's Underwritten Lease Base Of ~$1.2B; Will Generate ~$1.1B Of Total Cash Proceeds
Extra DataA:800015:L:en:K:0.42:C:0.4189840257167816

News ID: BZ$172e1882
Time of Article: 1713205827000
Headline: $100 Invested In This Stock 5 Years Ago Would Be Worth $500 Today
Extra Data: A:800015:L:en:K:n/a:C:0.8727717995643616

News ID: BZ$172e1892
Time of Article: 1713205831000
Headline: How Is The Market Feeling About Schlumberger?
Extra Data: A:800015:L:en:K:n/a:C:0.879279613494873

News ID: BZ$172e1897
Time of Article: 1713205837000
Headline: S&P 500 Down Over 1%; US Retail Sales Increase 0.7% In March
Extra Data: A:800015:L:en:K:0.97:C:0.97

News ID: BZ$172e1904
Time of Article: 1713205860000
Headline: Cantor Fitzgerald Reiterates Overweight on Eli Lilly and Co, Maintains $815 Price Target
Extra Data: A:800015:L:en:K:0.97:C:0.97

News ID: BZ$172e1fdb
Time of Article: 1713206215000
Headline: Biohaven shares are trading lower after the company announced the presentatino of data at the 2024 American Academy of Neurology Annual Meeting this weekend.
Extra Data: A:800015:L:en:K:n/a:C:0.7615593671798706

*/