#include "DBReadyCandle.h"

CandleTags::CandleTags(std::shared_ptr<Candle> c, TimeFrame tf, OptionType optType, TimeOfDay tod,
    VolumeStDev volStDev, VolumeThreshold volThresh, PriceDelta optPriceDelta,
    DailyHighsAndLows optDHL, LocalHighsAndLows optLHL) :
    candle(*c), tf_(tf), optType_(optType), tod_(tod), optPriceDelta_(optPriceDelta),
    volStDev_(volStDev), volThresh_(volThresh), optDHL_(optDHL), optLHL_(optLHL) {}

void CandleTags::setSqlId(int val) { sqlId = val; }

void CandleTags::addUnderlyingTags(RelativeToMoney rtm, PriceDelta pd, DailyHighsAndLows DHL, LocalHighsAndLows LHL) {
    rtm_ = rtm;
    underlyingPriceDelta_ = pd;
    underlyingDHL_ = DHL;
    underlyingLHL_ = LHL;
}

// Accessors
int CandleTags::getSqlId() const { return sqlId; }
TimeFrame CandleTags::getTimeFrame() const { return tf_; }
OptionType CandleTags::getOptType() const { return optType_; }
TimeOfDay CandleTags::getTOD() const { return tod_; }
RelativeToMoney CandleTags::getRTM() const { return rtm_; }
VolumeStDev CandleTags::getVolStDev() const { return volStDev_; }
VolumeThreshold CandleTags::getVolThresh() const { return volThresh_; }
PriceDelta CandleTags::getOptPriceDelta() const { return optPriceDelta_; }
DailyHighsAndLows CandleTags::getDHL() const { return optDHL_; }
LocalHighsAndLows CandleTags::getLHL() const { return optLHL_; }
PriceDelta CandleTags::getUnderlyingPriceDelta() const { return underlyingPriceDelta_; }
DailyHighsAndLows CandleTags::getUnderlyingDHL() const { return underlyingDHL_; }
LocalHighsAndLows CandleTags::getUnderlyingLHL() const { return underlyingLHL_; }