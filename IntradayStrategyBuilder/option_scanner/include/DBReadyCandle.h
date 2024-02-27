#ifndef DBREADYCANDLE_H
#define DBREADYCANDLE_H

#include <memory>
#include <vector>

#include "OptionEnums.h"
#include "Candle.h"

using namespace isb_option_tags;

// Purpose is to wrap candle sticks with attached tags for an SQL friendly format
// This will contain all tags available to be obtained upon the creation of a candle
// This allows for the candle itself to remain lightweight while this data is sent to the db
// or sent via callback to the alert handler
class CandleTags {
public:
    CandleTags(std::shared_ptr<Candle> c, TimeFrame tf, OptionType optType, TimeOfDay tod,
        VolumeStDev volStDev, VolumeThreshold volThresh, PriceDelta optPriceDelta,
        DailyHighsAndLows optDHL, LocalHighsAndLows optLHL);

    // Constructor if receiving db data
    //CandleTags(std::shared_ptr<Candle> c, std::vector<int> tags);

    void setSqlId(int val);
    // Mutator to add underlying tags
    void addUnderlyingTags(RelativeToMoney rtm, PriceDelta pd, DailyHighsAndLows DHL, LocalHighsAndLows LHL);

    // Make the candle a public member 
    Candle candle;

    int getSqlId() const;
    TimeFrame getTimeFrame() const;
    OptionType getOptType() const;
    TimeOfDay getTOD() const;
    RelativeToMoney getRTM() const;
    VolumeStDev getVolStDev() const;
    VolumeThreshold getVolThresh() const;
    PriceDelta getOptPriceDelta() const;
    DailyHighsAndLows getDHL() const;
    LocalHighsAndLows getLHL() const;
    PriceDelta getUnderlyingPriceDelta() const;
    DailyHighsAndLows getUnderlyingDHL() const;
    LocalHighsAndLows getUnderlyingLHL() const;
    VolumeROC getVolumeROC() const;
    PriceROC getPriceRoc() const;
    TrendingDirectionIntraday getIntradayTrend() const;
    TrendingDirectionDaily getDailyTrend() const;

private:
    std::vector<int> tags_{};
    int sqlId{ 0 };

    TimeFrame tf_{ TimeFrame::NoValue };
    OptionType optType_{ OptionType::NoValue };
    TimeOfDay tod_{ TimeOfDay::NoValue };

    VolumeStDev volStDev_{ VolumeStDev::NoValue };
    VolumeThreshold volThresh_{ VolumeThreshold::NoValue };
    PriceDelta optPriceDelta_{ PriceDelta::NoValue };
    DailyHighsAndLows optDHL_{ DailyHighsAndLows::NoValue };
    LocalHighsAndLows optLHL_{ LocalHighsAndLows::NoValue };

    // Tags that rely on underlying price data that will be updated after construction
    RelativeToMoney rtm_{ RelativeToMoney::NoValue };
    PriceDelta underlyingPriceDelta_{ PriceDelta::NoValue };
    DailyHighsAndLows underlyingDHL_{ DailyHighsAndLows::NoValue };
    LocalHighsAndLows underlyingLHL_{ LocalHighsAndLows::NoValue };

    VolumeROC vroc_{ VolumeROC::NoValue };
    PriceROC proc_{ PriceROC::NoValue };
    TrendingDirectionIntraday trendIntra_{ TrendingDirectionIntraday::NoValue };
    TrendingDirectionDaily trendDaily_{ TrendingDirectionDaily::NoValue };
};

#endif