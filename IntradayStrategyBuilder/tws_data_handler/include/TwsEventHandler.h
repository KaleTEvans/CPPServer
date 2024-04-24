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
    TickPriceInfo,
    TickSizeInfo,
    TickGenericInfo,
    TickStringInfo,
    TickNewsInfo,
    TickOptionInfo
};

// Base event type
class DataEvent {
    public:
        int reqId{0};
        DataEvent(int reqId) : reqId(reqId) {}
        virtual ~DataEvent() = default;
        virtual EventType getType() const = 0;
        virtual int getReqId() const;
};

//==============================================================
// Candle event - use for live candlestick data
//==============================================================

class CandleDataEvent : public DataEvent {
    public:
        std::shared_ptr<Candle> candle;
        CandleDataEvent(std::shared_ptr<Candle> candle) : candle(candle), DataEvent(candle->reqId()) {}
        virtual EventType getType() const override;
};

//==============================================================
// Tick Events - Several types of ticks, see docs 
//==============================================================

class TickDataEvent : public DataEvent {
    public:
        long timeStamp{0};
        TickType tickType{TickType::NOT_SET};
        TickDataEvent(int reqId, long timeStamp, TickType tickType = TickType::NOT_SET) : 
            DataEvent(reqId), timeStamp(timeStamp), tickType(tickType) {}
};

class TickPriceEvent : public TickDataEvent {
    public:
        double price{0};
        TickAttrib attrib;
        TickPriceEvent(int reqId, long timeStamp, TickType tickType, double price, const TickAttrib& attrib) :
            TickDataEvent(reqId, timeStamp, tickType), price(price), attrib(attrib) {}

        virtual EventType getType() const override;
};

class TickSizeEvent : public TickDataEvent {
    public:
        Decimal size{0};
        TickSizeEvent(int reqId, long timeStamp, TickType tickType, Decimal size) :
            TickDataEvent(reqId, timeStamp, tickType), size(size) {}

        virtual EventType getType() const override;
};

class TickGenericEvent : public TickDataEvent {
    public:
        double value{0};
        TickGenericEvent(int reqId, long timeStamp, TickType tickType, double value) :
            TickDataEvent(reqId, timeStamp, tickType), value(value) {}

        virtual EventType getType() const override;
};

class TickStringEvent : public TickDataEvent {
    public:
        std::string value{""};
        TickStringEvent(int reqId, long timeStamp, TickType tickType, std::string value) :
            TickDataEvent(reqId, timeStamp, tickType), value(value) {}

        virtual EventType getType() const override;
};

class TickNewsEvent : public DataEvent {
    public:
        time_t dateTime{0};
        std::string providerCode{""};
        std::string articleId{""};
        std::string headline{""};
        std::string extraData{""};
        TickNewsEvent(int reqId, time_t dateTime, std::string providerCode, std::string articleId,
            std::string headline, std::string extraData) : DataEvent(reqId), dateTime(dateTime), 
            providerCode(providerCode), articleId(articleId), headline(headline), extraData(extraData) {}

        virtual EventType getType() const override;
};

class TickOptionComputationEvent : public TickDataEvent {
    public:
        int tickAttrib{0};
        double impliedVol{0};
        double delta{0};
        double optPrice{0};
        double pvDividend{0};
        double gamma{0};
        double vega{0};
        double theta{0};
        double undPrice{0};
        TickOptionComputationEvent(int reqId, long timeStamp, TickType tickType, int tickAttrib, double impliedVol, double delta,
            double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice) :
            TickDataEvent(reqId, timeStamp, tickType), tickAttrib(tickAttrib), impliedVol(impliedVol), delta(delta), 
            optPrice(optPrice), pvDividend(pvDividend), gamma(gamma), vega(vega), theta(theta), undPrice(undPrice) {}

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