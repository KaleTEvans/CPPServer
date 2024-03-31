#ifndef TWSEVENTHANDLER_H
#define TWSEVENTHANDLER_H

////////////////////////////////////////////////////////////////////////////////////
// Event Handling
// These methods will handle all data events where the wrapper receives data
// and send to the appropriate modules
///////////////////////////////////////////////////////////////////////////////////

#include "Candle.h"
#include "TwsApiL0.h"
#include "TwsApiDefs.h"

#include <queue>
#include <functional>
#include <map>
#include <memory>
#include <mutex>

// Types of data that are recieved by the wrapper
enum class EventType {
    HistoricalCandleData,
    RealTimeCandleData,
    ContractInfo,
    EndOfRequest
};

// Base event type
class DataEvent {
    public:
        virtual ~DataEvent() = default;
        virtual EventType getType() const = 0;
};

// Candle event - use for historical and live candlestick data
class CandleDataEvent : public DataEvent {
    public:
        int reqId = 0;
        std::shared_ptr<Candle> candle;
        CandleDataEvent(int reqId, std::shared_ptr<Candle> candle) : reqId(reqId), candle(candle) {}
};

class HistoricalCandleDataEvent : public CandleDataEvent {
    public:
        HistoricalCandleDataEvent(int reqId, std::shared_ptr<Candle> candle)
            : CandleDataEvent(reqId, candle) {}

        virtual EventType getType() const override;
};
class RealTimeCandleDataEvent : public CandleDataEvent {
    public:
        RealTimeCandleDataEvent(int reqId, std::shared_ptr<Candle> candle)
            : CandleDataEvent(reqId, candle) {}

        virtual EventType getType() const override;
};

class ContractDataEvent : public DataEvent {
    public:
        int reqId = 0;
        ContractDetails details;
        ContractDataEvent(int reqId, ContractDetails details) : reqId(reqId), details(details) {}
        virtual EventType getType() const override;
};

class EndOfRequestEvent : public DataEvent {
    public:
        int reqId = 0;
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