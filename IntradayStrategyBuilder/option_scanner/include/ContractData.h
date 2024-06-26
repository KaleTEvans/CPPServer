#ifndef CONTRACTDATA_H
#define CONTRACTDATA_H

#include "TwsWrapper.h"
#include "TwsApiDefs.h"
#include "OptionEnums.h"

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

    std::shared_ptr<TickPriceEvent> tickPrice = nullptr;
    std::shared_ptr<TickGenericEvent> tickGeneric = nullptr;
    std::shared_ptr<TickSizeEvent> tickSize = nullptr;
    std::shared_ptr<TickStringEvent> tickString = nullptr;
    std::shared_ptr<TickOptionComputationEvent> tickOption = nullptr;
    std::shared_ptr<TimeAndSales> timeAndSales = nullptr;

    int64_t timestamp{0};
    void printMktData();
};

/////////////////////////////////////////////////////////
// The following class will contain all data for a 
// single five second frame of time
/////////////////////////////////////////////////////////

struct FiveSecondData {
    FiveSecondData(std::shared_ptr<Candle> candle, RelativeToMoney rtm);

    std::map<int64_t, MarketDataSingleFrame> ticks;

    std::shared_ptr<Candle> candle;
    RelativeToMoney rtm;

    // If we do get a news event during the time frame, be sure to include it for later analysis
    std::vector<std::shared_ptr<TickNewsEvent>> tickNews;
};

///////////////////////////////////////////////////
// Higher time frames for data anlysis
//////////////////////////////////////////////////

struct OneMinuteData {
    OneMinuteData(std::vector<std::shared_ptr<FiveSecondData>> candles, std::shared_ptr<TickOptionComputationEvent> optionInfo);

    RelativeToMoney rtm;
    // Add news ticks from five sec data
    std::vector<std::shared_ptr<TickNewsEvent>> tickNews;
    std::shared_ptr<Candle> candle;

    double impliedVol{0};
    double delta{0};
    double optPrice{0};
    double gamma{0};
    double vega{0};
    double theta{0};
    double undPrice{0};
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
        ContractData(std::shared_ptr<tWrapper> wrapper, int mktDataId, int rtbId, Contract contract, double strikeIncrement);

        void printData();

    private:
        int mktDataId;
        int rtbId;
        Contract contract;
        double strikeIncrement;
        double lastUnderlyingPrice{0};
        std::shared_ptr<tWrapper> wrapper;

        // One map will hold all ticks, the other the candles, and the ticks will be dispersed upon
        // creation of each candle
        std::map<int64_t, MarketDataSingleFrame> ticks;
        // Tick news will not be included in single frame data, as many articles can have the same timestamp
        std::vector<std::pair<int64_t, std::shared_ptr<TickNewsEvent>>> newsTicks;
        std::map<int64_t, std::shared_ptr<FiveSecondData>> fiveSecData;
        
        std::vector<std::shared_ptr<FiveSecondData>> fiveSecCandles;
        std::vector<std::shared_ptr<OneMinuteData>> oneMinuteCandles;

        // Event handlers
        void handleTickPriceEvent(std::shared_ptr<TickPriceEvent> event);
        void handleTickGenericEvent(std::shared_ptr<TickGenericEvent> event);
        void handleTickSizeEvent(std::shared_ptr<TickSizeEvent> event);
        void handleTickStringEvent(std::shared_ptr<TickStringEvent> event);
        void tickOptionInfo(std::shared_ptr<TickOptionComputationEvent> event);
        void tickNewsEvent(std::shared_ptr<TickNewsEvent> event);
        void realTimeCandles(std::shared_ptr<CandleDataEvent> event);
};

#endif