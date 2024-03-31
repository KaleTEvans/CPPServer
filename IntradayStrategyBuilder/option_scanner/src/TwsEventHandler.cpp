#include "TwsEventHandler.h"

EventType HistoricalCandleDataEvent::getType() const { return EventType::HistoricalCandleData; }
EventType RealTimeCandleDataEvent::getType() const { return EventType::RealTimeCandleData; }
EventType ContractDataEvent::getType() const { return EventType::ContractInfo; }
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