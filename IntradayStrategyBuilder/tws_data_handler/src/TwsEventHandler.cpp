#include "TwsEventHandler.h"

#include <sstream>
#include <string>

TickNewsEvent::TickNewsEvent(int reqId, time_t dateTime, std::string providerCode, std::string articleId,
    std::string headline, std::string extraData) : TickDataEvent(reqId), dateTime(dateTime), 
    providerCode(providerCode), articleId(articleId), headline(headline), extraData(extraData) {
        // Break apart news string into useful info
        std::string str = extraData;
        std::stringstream ss(str);

        // getline(ss, newsId, ' ');
        // getline(ss, dateTime, ' ');
        // getline(ss, newsSource, ' ');
        // getline(ss, headline);
    }

EventType HistoricalCandleDataEvent::getType() const { return EventType::HistoricalCandleData; }
EventType RealTimeCandleDataEvent::getType() const { return EventType::RealTimeCandleData; }
EventType ContractDataEvent::getType() const { return EventType::ContractInfo; }
EventType TickPriceEvent::getType() const { return EventType::TickPriceInfo; }
EventType TickSizeEvent::getType() const { return EventType::TickSizeInfo; }
EventType TickGenericEvent::getType() const { return EventType::TickGenericInfo; }
EventType TickNewsEvent::getType() const { return EventType::TickNewsInfo; }
EventType EndOfRequestEvent::getType() const { return EventType::EndOfRequest; }

MessageBus::MessageBus() {
    std::cout << "Message bus starting engines" << std::endl;
}

void MessageBus::subscribe(EventType type, std::function<void(std::shared_ptr<DataEvent>)> listener) {
    std::lock_guard<std::mutex> lock(mtx);
    listeners[type].push_back(listener);
    std::cout << "Listener subscribed" << std::endl;
}

void MessageBus::publish(std::shared_ptr<DataEvent> event) {
    // Likely unnecessary, but ensuring thread safe dispatching to listeners by creating a local copy
    std::vector<std::function<void(std::shared_ptr<DataEvent>)>> listenerList;

    std::unique_lock<std::mutex> lock(mtx);
    listenerList = listeners[event->getType()];
    lock.unlock();

    for (auto& listener : listenerList) {
        listener(event);
    }
}