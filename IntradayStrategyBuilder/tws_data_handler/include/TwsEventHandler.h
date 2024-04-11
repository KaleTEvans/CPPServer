#ifndef TWSEVENTHANDLER_H
#define TWSEVENTHANDLER_H

////////////////////////////////////////////////////////////////////////////////////
// Event Handling
// These methods will handle all data events where the wrapper receives data
// and send to the appropriate modules
///////////////////////////////////////////////////////////////////////////////////

#include "Candle.h"
#include "Contract.h"


#include <queue>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <set>

// Types of data that are recieved by the wrapper
enum class EventType {
    RealTimeCandleData,
    ContractInfo,
    ContractStrikesInfo,
    TickPriceInfo,
    TickSizeInfo,
    TickGenericInfo,
    TickNewsInfo,
    EndOfRequest
};

// Base event type
class DataEvent {
    public:
        virtual ~DataEvent() = default;
        virtual EventType getType() const = 0;
};

//==============================================================
// Candle event - use for historical and live candlestick data
//==============================================================

class CandleDataEvent : public DataEvent {
    public:
        std::shared_ptr<Candle> candle;
        CandleDataEvent(std::shared_ptr<Candle> candle) : candle(candle) {}
        virtual EventType getType() const override;
};

//==============================================================
// Contract Data Event
//==============================================================

class ContractDataEvent : public DataEvent {
    public:
        int reqId{0};
        ContractDetails details;
        ContractDataEvent(int reqId, ContractDetails details) : reqId(reqId), details(details) {}
        virtual EventType getType() const override;
};

class ContractOptStrikesEvent : public DataEvent {
    public:
        int reqId{0};
        std::string exchange{""};
        std::string tradingClass{""};
        std::set<double> strikes;
        ContractOptStrikesEvent(int reqId, std::string exchange, std::string tradingClass, std::set<double> strikes) :
            reqId(reqId), exchange(exchange), tradingClass(tradingClass), strikes(strikes) {}
        virtual EventType getType() const override;
};

//==============================================================
// Tick Events - Several types of ticks, see docs 
//==============================================================

class TickDataEvent : public DataEvent {
    public:
        int reqId{0};
        TickType tickType{TickType::NOT_SET};
        TickDataEvent(int reqId, TickType tickType = TickType::NOT_SET) : reqId(reqId), tickType(tickType) {}
};

class TickPriceEvent : public TickDataEvent {
    public:
        double price{0};
        TickAttrib attrib;
        TickPriceEvent(int reqId, TickType tickType, double price, const TickAttrib& attrib) :
            TickDataEvent(reqId, tickType), price(price), attrib(attrib) {}

        virtual EventType getType() const override;
};

class TickSizeEvent : public TickDataEvent {
    public:
        int size{0};
        TickSizeEvent(int reqId, TickType tickType, int size) :
            TickDataEvent(reqId, tickType), size(size) {}

        virtual EventType getType() const override;
};

class TickGenericEvent : public TickDataEvent {
    public:
        double value{0};
        TickGenericEvent(int reqId, TickType tickType, double value) :
            TickDataEvent(reqId, tickType), value(value) {}

        virtual EventType getType() const override;
};

class TickNewsEvent : public TickDataEvent {
    public:
        TickType tickType{TickType::NOT_SET};
        time_t dateTime{0};
        std::string providerCode{""};
        std::string articleId{""};
        std::string headline{""};
        std::string extraData{""};
        TickNewsEvent(int reqId, time_t dateTime, std::string providerCode, std::string articleId,
            std::string headline, std::string extraData); // Constructor defined in cpp file

        virtual EventType getType() const override;
};

class EndOfRequestEvent : public DataEvent {
    public:
        int reqId{0};
        EndOfRequestEvent(int reqId) : reqId(reqId) {}

        virtual EventType getType() const override;
};

//======================================================================================
// Message bus will handle each type of data receied from the wrapper
// Multiple modules will be able to subscribe to the message bus and listen for updates 
// to their requested event type
//======================================================================================

class MessageBus {
    public:
        MessageBus();

        void subscribe(EventType type, std::function<void(std::shared_ptr<DataEvent>)> listener);

        // Each subscriber will receive all data associated with the EventType, and must filter accordingly

        void publish(std::shared_ptr<DataEvent> event);

    private:
        // Maps to each type of event data, containing the list of subscribers for each type
        std::map<EventType, std::vector<std::function<void(std::shared_ptr<DataEvent>)>>> listeners;

        std::mutex mtx;
};

#endif