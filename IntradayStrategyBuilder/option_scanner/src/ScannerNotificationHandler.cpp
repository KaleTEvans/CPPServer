#include "ScannerNotificationHandler.h"

#include <sstream>
#include <string>

ScannerNotifications HighActivityEvent::getNotificationType() const { return ScannerNotifications::HighActivity; }
ScannerNotifications HighVolumeEvent::getNotificationType() const { return ScannerNotifications::HighVolumeLowPriceChange; }
ScannerNotifications LargeOrderEvent::getNotificationType() const { return ScannerNotifications::LargeOrder; }

std::string LargeOrderEvent::formatCSV() {
    return std::to_string(timeStamp) + "," +
            std::to_string(con.strike) + "," +
            con.right + "," +
            std::to_string(priceOfSale) + "," +
            std::to_string(quantityOfSale) + "," + 
            std::to_string(totalSaleValue) + "," +
            std::to_string(totalVol) + "," +
            std::to_string(VWAP) + "," +
            std::to_string(currentAsk) + "," +
            std::to_string(currentBid) + "," +
            getRTMstr(rtm) + "\n";
}

void LargeOrderEvent::printLargeOrder() {
    std::cout << timeStamp << " | Option: " << con.strike << con.right << " | Purchase Price: $" << priceOfSale <<
        " | Quantity: " << quantityOfSale << " | Total Sale: $" << totalSaleValue <<
        " | Bid: $" << currentBid << " | Ask: $" << currentAsk << " | " << getRTMstr(rtm) << std::endl; 
}

void ScannerNotificationBus::subscribe(ScannerNotifications scnType, std::function<void(std::shared_ptr<ScannerEvent>)> listener) {
    std::lock_guard<std::mutex> lock(mtx);
    listeners[scnType].push_back(listener);
    std::cout << "Option Scanner listener subscribed" << std::endl;
}

void ScannerNotificationBus::publish(std::shared_ptr<ScannerEvent> event) {
    std::vector<std::function<void(std::shared_ptr<ScannerEvent>)>> listenerList;

    std::unique_lock<std::mutex> lock(mtx);
    listenerList = listeners[event->getNotificationType()];
    lock.unlock();

    for (auto& listener : listenerList) {
        listener(event);
    }
}