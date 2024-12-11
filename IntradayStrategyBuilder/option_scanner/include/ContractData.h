#ifndef CONTRACTDATA_H
#define CONTRACTDATA_H

#include "TwsWrapper.h"
#include "TwsApiDefs.h"
#include "OptionEnums.h"
#include "OptionStatisticTrackers.h"
#include "SaveToCSV.h"
#include "ScannerNotificationHandler.h"
#include "SocketDataCollector.h"
#include "../generated/messages.pb.h"

#include <set>
#include <map>
#include <cmath>
#include <algorithm>
#include <queue>

using namespace TwsApi;
using namespace isb_option_tags;


/*******************************************
    Based on data output, should aggregate ask in bid price from ticks
    Then use RTVolume and compare to current bid/ask to determine
    whether it is a buy or sell, then create method to verify this against realtime bars
    - Should just use 5 second snapshots of greeks
    - Could just monitor ask and bid size for anomalies, such as values over 100
    - ** Tick volume appears to lag behind RTVolume significantly
    - ** Real TIme Candles appear to be 5 seconds delayed
*/

///////////////////////////////////////////////////////
// Holds time and sales data for RTVolume
///////////////////////////////////////////////////////

class TimeAndSales {
    public:
        TimeAndSales(std::string data);
        void print();

        std::string serializeTimeAndSales(const Contract con, const RelativeToMoney rtm,
            double currentAsk, double currentBid);

        std::string data;
        double price{0};
        int quantity{0};
        long timeValue{0};
        int totalVol{0};
        double vwap{0};
        bool filledBySingleMM{false};
};

//////////////////////////////////////////////////////
// Will map all ticks within a five second frame
// at a milisecond level of granularity
//////////////////////////////////////////////////////

struct MarketDataSingleFrame {
    MarketDataSingleFrame(int64_t timestamp);

    void inputTickPrice(std::shared_ptr<TickPriceEvent> tickPrice);
    void inputTickSize(std::shared_ptr<TickSizeEvent> tickSize);
    void inputTickOption(std::shared_ptr<TickOptionComputationEvent> tickOption);
    void inputTimeAndSales(std::shared_ptr<TimeAndSales> timeAndSales);

    std::string valueToCSV(double value);
    std::string formatCSV(RelativeToMoney rtm = RelativeToMoney::NoValue);
    std::string serializeTickData(const Contract con);

    // Variables to track. Items not present will be set to a -1 value aside from greeks, which will be -100
    double bidPrice{-1};
    double bidSize{-1};
    double askPrice{-1};
    double askSize{-1};
    double lastPrice{-1};
    double markPrice{-1};
    double volume{-1};
    double impliedVol{-100};
    double delta{-100};
    double gamma{-100};
    double vega{-100};
    double theta{-100};
    double undPrice{-1};
    double tasPrice{-1};
    double tasQuantity{-1};
    double tasTotalVol{-1};
    double tasVWAP{-1};
    bool tasFilledBySingleMM{false};

    int64_t timestamp{0};
};

/////////////////////////////////////////////////////////
// The following class will contain all data for a 
// single five second frame of time
/////////////////////////////////////////////////////////

struct FiveSecondData {
    FiveSecondData(std::shared_ptr<Candle> candle, RelativeToMoney rtm);

    std::string formatCSV();
    std::string serializeFiveSecData(const Contract con, const RelativeToMoney rtm);
    
    std::map<int64_t, std::shared_ptr<MarketDataSingleFrame>> ticks;

    std::shared_ptr<Candle> candle;
    RelativeToMoney rtm;

    // If we do get a news event during the time frame, be sure to include it for later analysis
    std::vector<std::shared_ptr<TickNewsEvent>> tickNews;
};

///////////////////////////////////////////////////
// Higher time frames for data anlysis
//////////////////////////////////////////////////

struct OneMinuteData {
    OneMinuteData(std::vector<std::shared_ptr<FiveSecondData>> candles, 
    std::shared_ptr<MarketDataSingleFrame> optionInfo, std::shared_ptr<MarketDataSingleFrame> tasInfo);

    std::string formatCSV();
    std::string serializeOneMinData(const Contract con, const RelativeToMoney rtm);

    RelativeToMoney rtm;
    std::shared_ptr<Candle> candle;

    double candleVol{0};
    double impliedVol{0};
    double delta{0};
    double gamma{0};
    double vega{0};
    double theta{0};
    double undPrice{0};
    double tradeCount{0};
    long totalVol{0};
};

///////////////////////////////////////////////////////////////////
// Contract Data is a method that will combine both tick data 
// and 5 second real time bar data
//
// This is the primary structure for monitoring live price data
// as well as consolidating into larger candlesticks for 
// further analysis
//////////////////////////////////////////////////////////////////

class ContractData {
    public:
        ContractData(std::shared_ptr<tWrapper> wrapper,
            std::shared_ptr<SocketDataCollector> sdc,
            std::shared_ptr<ScannerNotificationBus> notifications,
            int mktDataId, int rtbId, Contract contract, double strikeIncrement);
        ~ContractData();

        void cancelDataStream();
        void printData();

        // Underlying Price will be updated from the option scnnaer in the case of slow incoming ticks
        void updateUnderlyingPrice(double price);

    private:
        int mktDataId;
        int rtbId;
        Contract contract;
        double strikeIncrement;
        double lastUnderlyingPrice{0};
        std::shared_ptr<tWrapper> wrapper;
        //std::shared_ptr<CSVFileSaver> csv;
        std::shared_ptr<SocketDataCollector> sdc;
        bool outputData{false};

        std::mutex mtx;

        double currentBid{0};
        double currentAsk{0};
        double currentIV{0};
        RelativeToMoney currentRtm{RelativeToMoney::NoValue};
        
        // Send any notifications via the bus
        std::shared_ptr<ScannerNotificationBus> notifications;

        // One map will hold all ticks, the other the candles, and the ticks will be dispersed upon
        // creation of each candle
        std::map<int64_t, std::shared_ptr<MarketDataSingleFrame>> ticks;
        std::map<int64_t, std::shared_ptr<FiveSecondData>> fiveSecData;
        std::vector<std::shared_ptr<OneMinuteData>> oneMinuteCandles;

        // For accumulating one minute candles
        std::vector<std::shared_ptr<FiveSecondData>> tempCandles;
        bool isEvenMinute{false};

        // Statistical factors to determine values of interest
        StandardDeviation tradeSize_timeAndSales;
        StandardDeviation tradeCount_fiveSecCandles;
        StandardDeviation priceDelta_fiveSecCandles;
        StandardDeviation priceDelta_oneMinCandles;

        // Event handlers
        void handleTickPriceEvent(std::shared_ptr<TickPriceEvent> event);
        void handleTickSizeEvent(std::shared_ptr<TickSizeEvent> event);
        void handleTickStringEvent(std::shared_ptr<TickStringEvent> event);
        void tickOptionInfo(std::shared_ptr<TickOptionComputationEvent> event);
        void realTimeCandles(std::shared_ptr<CandleDataEvent> event);
};

#endif