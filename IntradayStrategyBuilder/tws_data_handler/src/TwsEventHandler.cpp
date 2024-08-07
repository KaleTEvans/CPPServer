#include "TwsEventHandler.h"

#include <sstream>
#include <string>

int DataEvent::getReqId() const { return reqId; }

EventType CandleDataEvent::getType() const { return EventType::RealTimeCandleData; }
EventType TickPriceEvent::getType() const { return EventType::TickPriceInfo; }
EventType TickSizeEvent::getType() const { return EventType::TickSizeInfo; }
EventType TickGenericEvent::getType() const { return EventType::TickGenericInfo; }
EventType TickStringEvent::getType() const { return EventType::TickStringInfo; }
EventType TickNewsEvent::getType() const { return EventType::TickNewsInfo; }
EventType TickOptionComputationEvent::getType() const { return EventType::TickOptionInfo; }

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