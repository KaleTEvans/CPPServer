#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>

#include "TwsWrapper.h"
#include "TwsApiDefs.h"

using namespace TwsApi;

const unsigned MAX_ATTEMPTS = 50;
const unsigned SLEEP_TIME = 10;

class Subscriber {
public:
    Subscriber(std::shared_ptr<tWrapper> wrapper) : wrapper(wrapper) {

        //=========================================================
        // Message Bus Subscription Methods
        //
        // Can be subscribed to here or on demand 
        // NOTE: A request must be called for 
        // the specific TWS data at least once
        //
        // Use wrapper request functions under 'Data
        // Streaming Eclient Functions' to start new data streams
        //=========================================================

        // Refer to the TwsEventHandler class for all event types sent to the message bus
        wrapper->getMessageBus()->subscribe(EventType::TickPriceInfo, [this](std::shared_ptr<DataEvent> event) {
            // Use dynamic_pointer_cast to cast Event type to child class type
            this->handleTickPriceEvent(std::dynamic_pointer_cast<TickPriceEvent>(event));
        });

        wrapper->getMessageBus()->subscribe(EventType::TickNewsInfo, [this](std::shared_ptr<DataEvent> event) {
            this->handleTickNewsEvent(std::dynamic_pointer_cast<TickNewsEvent>(event));
        });

        wrapper->getMessageBus()->subscribe(EventType::RealTimeCandleData, [this](std::shared_ptr<DataEvent> event) {
            this->realTimeCandles(std::dynamic_pointer_cast<CandleDataEvent>(event));
        });

        wrapper->getMessageBus()->subscribe(EventType::TickOptionInfo, [this](std::shared_ptr<DataEvent> event) {
            this->tickOptionInfo(std::dynamic_pointer_cast<TickOptionComputationEvent>(event));
        });

        wrapper->getMessageBus()->subscribe(EventType::TickGenericInfo, [this](std::shared_ptr<DataEvent> event) {
            this->handleTickGenericEvent(std::dynamic_pointer_cast<TickGenericEvent>(event));
        });

        wrapper->getMessageBus()->subscribe(EventType::TickSizeInfo, [this](std::shared_ptr<DataEvent> event) {
            this->handleTickSizeEvent(std::dynamic_pointer_cast<TickSizeEvent>(event));
        });
    }

    // Wrapper acess, use for sending requests
    std::shared_ptr<tWrapper> getWrapper() { return wrapper; }

    //=========================================
    // Message Bus Callback Functions
    //
    // All data returned as shared pointers
    // allows for easy cleanup of unused data
    //=========================================
    double lastPrice = 0;

    void handleTickPriceEvent(std::shared_ptr<TickPriceEvent> event) {
        printf( "Tick Price. Ticker Id: %d, Field: %s, Price: %f, CanAutoExecute: %d, PastLimit: %d, PreOpen: %d\n", 
        event->reqId, *(TickTypes::ENUMS)event->tickType, event->price, event->attrib.canAutoExecute, event->attrib.pastLimit, event->attrib.preOpen);
        if (event->tickType == 4) lastPrice = event->price;
    }

    void handleTickNewsEvent(std::shared_ptr<TickNewsEvent> event) {
        std::cout << "News ID: " << event->articleId << std::endl;
        std::cout << "Time of Article: " << event->dateTime << std::endl;
        std::cout << "Headline: " << event->headline << std::endl;
        std::cout << "Extra Data: " << event->extraData << std::endl;
        
        // Parse the extra data string to get the sentiment score
        std::istringstream ss(event->extraData);
        std::string token;
        std::string score;
        // Need to account for no K in field
        while (std::getline(ss, token, ':')) {
            std::string key = token;
            if (!std::getline(ss, token, ':')) break;

            std::string val = token;
            if (key == "K") score = val;
        }
        double scoreVal = 0;
        if (score != "n/a") scoreVal = std::stod(score);
        std::cout << "Sentiment Score: " << scoreVal << std::endl;
    }

    void realTimeCandles(std::shared_ptr<CandleDataEvent> event) {
        event->candle->printCandle();
    }

    void tickOptionInfo(std::shared_ptr<TickOptionComputationEvent> event) {
        printf("TickOptionComputation. Ticker Id: %d, Type: %s, TickAttrib: %d," 
        "ImpliedVolatility: %f, Delta: %f, OptionPrice: %f, pvDividend: %f, Gamma: %f," 
        "Vega: %f, Theta: %f, Underlying Price: %f\n", event->reqId, *(TickTypes::ENUMS)event->tickType, 
        event->tickAttrib, event->impliedVol, event->delta, event->optPrice, event->pvDividend, 
        event->gamma, event->vega, event->theta, event->undPrice);
    }

    void handleTickGenericEvent(std::shared_ptr<TickGenericEvent> event) {
        printf( "Tick Generic. Ticker Id: %d, Type: %s, Value: %f\n", 
            event->reqId, *(TickTypes::ENUMS)event->tickType, event->value);
    }

    void handleTickSizeEvent(std::shared_ptr<TickSizeEvent> event) {
        printf( "Tick Size. Ticker Id: %d, Field: %s, Size: %s\n", 
            event->reqId, *(TickTypes::ENUMS)event->tickType, decimalStringToDisplay(event->size).c_str());
    }

    //===============================================
    // Single Event / Batch Callbacks
    //
    // Returns an int, which is the request ID 
    // associated with the specific request each time
    // it is called. Use the wrapper function 
    // checkEventCompleted() to determine if all 
    // batch data has been received 
    //===============================================

    int getContractData(const Contract& con) {
        int reqId = 0;
        wrapper->reqContractDetails(
            [this](int reqId) {
                reqId = reqId;
            },
            [this](const ContractDetails& contractDetails) {
            this->handleContractDataEvent(contractDetails);
        }, con);
        return reqId;
    }

    // Will return a chain for every exchange the security trades on
    int getOptionsChain(const std::string& underlyingSymbol, const std::string& futFopExchange, 
        const std::string& underlyingSecType, int underlyingConId) {
            int reqId = 0;
            wrapper->reqSecDefOptParams(
                [this](int reqId) {
                    reqId = reqId;
                },
                [this](const std::string& exchange, int underlyingConId, const std::string& tradingClass, 
                    const std::string& multiplier, const std::set<std::string>& expirations, 
                    const std::set<double>& strikes) {
                        this->handleOptionsChainData(exchange, underlyingConId, tradingClass, multiplier,
                            expirations, strikes);
                    }, 
                underlyingSymbol, futFopExchange, underlyingSecType, underlyingConId
            );
            return reqId;
        }

    int getHistoricalData(const Contract &con, const std::string &endDateTime, 
        const std::string &durationStr, const std::string &barSizeSetting, const std::string &whatToShow, 
        int useRTH, int formatDate, bool keepUpToDate) {
            int reqId = 0;
            wrapper->reqHistoricalData(
                [this](int reqId) {
                    reqId = reqId;
                },
                [this](std::shared_ptr<Candle> candle) {
                    this->handleHistoricalData(candle);
                },
                con, endDateTime, durationStr, barSizeSetting, whatToShow, useRTH, formatDate, keepUpToDate
            );
            return reqId;
    }

    //================================================
    // Data Handlers For Single Events
    //
    // Now you can send this information elsewhere
    //================================================

    void handleContractDataEvent(const ContractDetails& contractDetails) {
        std::cout << "Con ID: " << contractDetails.contract.conId << std::endl;
        std::cout << contractDetails.contract.description << std::endl;
    }

    void handleOptionsChainData(const std::string& exchange, 
        int underlyingConId, const std::string& tradingClass, const std::string& multiplier, 
        const std::set<std::string>& expirations, const std::set<double>& strikes) {
            std::vector<double> sortedStrikes;
            for (auto& i : strikes) {
                sortedStrikes.push_back(i);
            }

            getClostestStrikes(sortedStrikes);
    }

    void getClostestStrikes(const std::vector<double>& strikes) {
        std::cout << "Strikes Received" << std::endl;
        sortedStrikes.clear();
        for (auto& i : strikes) sortedStrikes.push_back(i);
    }

    void handleHistoricalData(std::shared_ptr<Candle> candle) {

    }

private:
    std::shared_ptr<tWrapper> wrapper;
    std::vector<double> sortedStrikes;
};