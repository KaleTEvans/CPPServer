#ifndef SCANNERNOTIFICATIONHANDLER_H
#define SCANNERNOTIFICATIONHANDLER_H

////////////////////////////////////////////////////////////////////////////////////
// Event Handling
// These methods will handle all data events if a requested notification is found
// while collecting data via contract data and the option scanner
///////////////////////////////////////////////////////////////////////////////////

#include "Candle.h"
#include "Contract.h"
#include "OptionEnums.h"

#include <queue>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <set>

using namespace isb_option_tags;

enum class ScannerNotifications {
    Momentum,
    HighActivity,
    HighVolumeLowPriceChange,
    LargeOrder,
    SaveDataOnly
};

class ScannerEvent {
    public:
        ScannerEvent(Contract con, long timeStamp, RelativeToMoney rtm) : 
            con(con), timeStamp(timeStamp), rtm(rtm) {}
        Contract con;
        long timeStamp{0};
        RelativeToMoney rtm{RelativeToMoney::NoValue};

        virtual ~ScannerEvent() = default;
        virtual ScannerNotifications getNotificationType() const = 0;
};

// class MomentumEvent : public ScannerEvent {
//     public:

// };

class HighActivityEvent : public ScannerEvent {
    public:
        double currentPrice{0};
        double averageVolLast5Minutes{0};
        double averageTradesLast5Minutes{0};
        double averageReturnLast5Minutes{0};
        double averageIVLast5Minutes{0};
        double totalVolLastMinute{0};
        double totalTradesLastMinute{0};
        double totalReturnLastMinute{0};
        double currentIV{0};

        HighActivityEvent(Contract con, long timeStamp, double currentPrice, double averageVolLast5Minutes,
            double averageTradesLast5Minutes, double averageReturnLast5Minutes, double averageIVLast5Minutes,
            double totalVolLastMinute, double totalTradesLastMinute, double totalReturnLastMinute,
            double currentIV, RelativeToMoney rtm) : ScannerEvent(con, timeStamp, rtm), currentPrice(currentPrice),
                averageVolLast5Minutes(averageVolLast5Minutes), averageTradesLast5Minutes(averageTradesLast5Minutes),
                averageReturnLast5Minutes(averageReturnLast5Minutes), averageIVLast5Minutes(averageIVLast5Minutes),
                totalVolLastMinute(totalVolLastMinute), totalTradesLastMinute(totalTradesLastMinute), 
                totalReturnLastMinute(totalReturnLastMinute), currentIV(currentIV) {}
        
        virtual ScannerNotifications getNotificationType() const override;
};

class HighVolumeEvent : public ScannerEvent {
    public:
        double currentPrice{0};
        double currentAverageVolume{0};
        double currentVolumeStandardDev{0};
        double currentAveragePriceReturn{0};
        double currentPriceReturnStandardDev{0};
        double currentVolume{0};
        double currentPriceReturn{0};

        HighVolumeEvent(Contract con, long timeStamp, double currentPrice, double currentAverageVolume, 
            double currentVolumeStandardDev, double currentAveragePriceReturn, double currentPriceReturnStandardDev,
            double currentVolume, double currentPriceReturn, RelativeToMoney rtm) :
                ScannerEvent(con, timeStamp, rtm), currentPrice(currentPrice), currentAverageVolume(currentAverageVolume),
                currentVolumeStandardDev(currentVolumeStandardDev), currentAveragePriceReturn(currentAveragePriceReturn),
                currentPriceReturnStandardDev(currentPriceReturnStandardDev), currentVolume(currentVolume),
                currentPriceReturn(currentPriceReturn) {}

        virtual ScannerNotifications getNotificationType() const override;
};

class LargeOrderEvent : public ScannerEvent {
    public:
        double priceOfSale{0};
        double quantityOfSale{0};
        double totalSaleValue{0};
        double totalVol{0};
        double VWAP{0};
        double currentAsk{0};
        double currentBid{0};

        LargeOrderEvent(Contract con, long timeStamp, double priceOfSale, double quantityOfSale, 
            double totalSaleValue, double totalVol, double VWAP, double currentAsk, 
            double currentBid, RelativeToMoney rtm) :
                ScannerEvent(con, timeStamp, rtm), priceOfSale(priceOfSale), quantityOfSale(quantityOfSale),
                totalSaleValue(totalSaleValue), totalVol(totalVol), VWAP(VWAP), 
                currentAsk(currentAsk), currentBid(currentBid) {}
        
        virtual ScannerNotifications getNotificationType() const override;
        std::string formatCSV();
};

class ScannerNotificationBus {
    public:
        // Alert event handling
        void subscribe(ScannerNotifications scnType, std::function<void(std::shared_ptr<ScannerEvent>)> listener);
        void publish(std::shared_ptr<ScannerEvent> event);

    private:
        std::map<ScannerNotifications, std::vector<std::function<void(std::shared_ptr<ScannerEvent>)>>> listeners;
        std::mutex mtx;
};

#endif