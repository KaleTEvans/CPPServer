#ifndef UNDERLYINGDATA_H
#define UNDERLYINGDATA_H

#include "TwsWrapper.h"
#include "SaveToCSV.h"
#include "ContractDefs.h"
#include "SocketDataCollector.h"
#include "../generated/messages.pb.h"

#include <fstream>

///////////////////////////////////////////////////////
// Underlying Data
// Will save news separately by tick, only will save
// 5 second price data, all other values will be
// requested once per minute
//
// The following ticks will be tracked once per minute:
// High / Low price of the day
// Volume of the day
// Current day call volume (reqId 100)
// Current day put volume (reqId 100)
// Implied Volatility 30-day (reqId 106)
// Historical Volatility 30-day (reqId 104)
// Index future premium (reqId 162)
// Trade count for the day (reqId 293)
// Trade rate per minute (reqId 294)
// Real time historical volatility 30-day (reqId 411)
//
// The following items will be captured for a daily chart
// 13 week low (reqId 165)
// 13 week high (reqId 165)
// 26 week low (reqId 165)
// 26 week high (reqId 165)
// 52 week low (reqId 165)
// 52 week high (reqId 165)
// Average Volume 90-day (reqId 165)
// Call option Open Interest (reqId 101)
// Put option Open Interest (reqId 101)
// Futures open interest (reqId 588)
////////////////////////////////////////////////////////

struct UnderlyingOneMinuteData {
    void addFiveSecData(std::vector<std::shared_ptr<Candle>> fiveSecData);

    long time{-1};
    double open{-1};
    double high{INT_MIN};
    double low{INT_MAX};
    double close{-1};
    double volume{-1};

    double dailyHigh{-1};
    double dailyLow{-1};
    double dailyVolume{-1};
    double totalCallVolume{-1};
    double totalPutVolume{-1};
    double indexFuturePremium{-1};
    double totalTradeCount{-1};
    double oneMinuteTradeRate{-1};
    double realTimeHistoricalVolatility{-100};
    double optionImpliedVolatility{-100};
    double optionHistoricalVolatility{-100};
    double callOpenInterest{-1};
    double putOpenInterest{-1};
    double futuresOpenInterest{-1};

    std::string formatCSV();
    std::string serializeOneMinData(const std::string& symbol);
};

struct ContractNewsData {
    long time{0};
    std::string articleId{""};
    std::string headline{""};
    double sentimentScore{-100};
    double price{0};

    std::string formatCSV();
    std::string serializeNewsObject();
    // Use to ignore commas in headlines to prevent column separation
    std::string escapeCommas(const std::string& value);
};


class UnderlyingData {
    public:
        UnderlyingData(std::shared_ptr<tWrapper> wrapper, std::shared_ptr<SocketDataCollector> sdc, Contract contract);
        ~UnderlyingData();

        Contract getContract();
        void startReceivingData();
        void stopReceivingData();
        int getStrikeIncrement();
        std::pair<std::vector<double>, std::vector<double>> getStrikes(int countITM = 2);
        std::string formatAveragesCSV();

    private:
        int requestOptionsChain();
        void handleTickPriceEvent(std::shared_ptr<TickPriceEvent> event);
        void handleTickSizeEvent(std::shared_ptr<TickSizeEvent> event);
        void handleTickGenericEvent(std::shared_ptr<TickGenericEvent> event);
        void handleTickNewsEvent(std::shared_ptr<TickNewsEvent> event);
        void handleRealTimeCandles(std::shared_ptr<CandleDataEvent> event);
        void handleOptionsChainData(const std::string& exchange, 
            int underlyingConId, const std::string& tradingClass, const std::string& multiplier, 
            const std::set<std::string>& expirations, const std::set<double>& strikes);

        std::string serializeKeyPricePoints();

        std::map<long, std::pair<std::shared_ptr<TickNewsEvent>, double>> newsTicks; // Paired with most recent underlying price

        std::vector<double> optionsChain;
        std::set<double> optionStrikes;
        std::vector<std::shared_ptr<Candle>> fiveSecData;
        std::vector<UnderlyingOneMinuteData> oneMinuteData;
        int mktDataId{0};
        int rtbId{0};
        Contract contract;
        double currentPrice{0};
        double strikeIncrement{0};
        std::shared_ptr<tWrapper> wrapper;
        //std::shared_ptr<CSVFileSaver> csv;
        std::shared_ptr<SocketDataCollector> sdc;

        double low13Week{-1};
        double high13Week{-1};
        double low26week{-1};
        double high26Week{-1};
        double low52Week{-1};
        double high52Week{-1};
        double averageVolume90Day{-1};

        bool verifyAveragesRecveived();

        bool outputData{false};
};

#endif