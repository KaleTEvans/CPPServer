#ifndef UNDERLYINGDATA_H
#define UNDERLYINGDATA_H

#include "TwsWrapper.h"
#include "SaveToCSV.h"
#include "ContractDefs.h"

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

    long time{0};
    double open{0};
    double high{INT_MIN};
    double low{INT_MAX};
    double close{0};
    double volume{0};

    double dailyHigh{0};
    double dailyLow{0};
    double dailyVolume{0};
    double totalCallVolume{0};
    double totalPutVolume{0};
    double indexFuturePremium{0};
    double totalTradeCount{0};
    double oneMinuteTradeRate{0};
    double realTimeHistoricalVolatility{0};
    double optionImpliedVolatility{0};
    double optionHistoricalVolatility{0};
    double callOpenInterest{0};
    double putOpenInterest{0};
    double futuresOpenInterest{0};

    std::string formatCSV();
};

struct ContractNewsData {
    long time{0};
    std::string articleId{""};
    std::string headline{""};
    double sentimentScore{0};
    double price{0};

    std::string formatCSV();
};


class UnderlyingData {
    public:
        UnderlyingData(std::shared_ptr<tWrapper> wrapper, std::shared_ptr<CSVFileSaver> csv, Contract contract);
        ~UnderlyingData();

        Contract getContract();
        void startReceivingData();
        void stopReceivingData();
        int getStrikeIncrement();
        std::pair<std::vector<double>, std::vector<double>> getStrikes(int countITM = 2);
        std::string formatAveragesCSV();

        void printData();

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

        std::map<long, std::pair<std::shared_ptr<TickNewsEvent>, double>> newsTicks; // Paired with most recent underlying price

        std::vector<double> optionsChain;
        std::set<double> optionStrikes;
        std::vector<std::shared_ptr<Candle>> fiveSecData;
        std::vector<UnderlyingOneMinuteData> oneMinuteData;
        int mktDataId{0};
        int rtbId{0};
        Contract contract;
        double currentPrice{5646.96};
        double strikeIncrement{0};
        std::shared_ptr<tWrapper> wrapper;
        std::shared_ptr<CSVFileSaver> csv;

        double low13Week{0};
        double high13Week{0};
        double low26week{0};
        double high26Week{0};
        double low52Week{0};
        double high52Week{0};
        double averageVolume90Day{0};

        bool outputData{false};
};

#endif