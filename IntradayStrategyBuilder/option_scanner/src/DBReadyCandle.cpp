#include "DBReadyCandle.h"

CandleTags::CandleTags(std::shared_ptr<Candle> c, TimeFrame tf, OptionType optType, TimeOfDay tod,
    VolumeStDev volStDev, VolumeThreshold volThresh, PriceDelta optPriceDelta,
    DailyHighsAndLows optDHL, LocalHighsAndLows optLHL) :
    candle(*c), tf_(tf), optType_(optType), tod_(tod), optPriceDelta_(optPriceDelta),
    volStDev_(volStDev), volThresh_(volThresh), optDHL_(optDHL), optLHL_(optLHL) {}

// CandleTags::CandleTags(std::shared_ptr<Candle> c, std::vector<int> tags) : candle(*c), tags_(tags)
// {
//     std::string tf = TagDBInterface::intToTag[tags[0]].first;
//     std::string opttype = TagDBInterface::intToTag[tags[1]].first;
//     std::string tod = TagDBInterface::intToTag[tags[2]].first;
//     std::string rtm = TagDBInterface::intToTag[tags[3]].first;
//     std::string volstdev = TagDBInterface::intToTag[tags[4]].first;
//     std::string volthresh = TagDBInterface::intToTag[tags[5]].first;
//     std::string priceDelta = TagDBInterface::intToTag[tags[6]].first;
//     std::string opt_dhl = TagDBInterface::intToTag[tags[7]].first;
//     std::string opt_lhl = TagDBInterface::intToTag[tags[8]].first;
//     std::string underlyingPriceDelta = TagDBInterface::intToTag[tags[9]].first;
//     std::string u_dhl = TagDBInterface::intToTag[tags[10]].first;
//     std::string u_lhl = TagDBInterface::intToTag[tags[11]].first;

//     tf_ = str_to_tf(tf);
//     optType_ = EnumString::str_to_option_type(opttype);
//     tod_ = EnumString::str_to_tod(tod);
//     rtm_ = EnumString::str_to_rtm(rtm);
//     volStDev_ = EnumString::str_to_vol_stdev(volstdev);
//     volThresh_ = EnumString::str_to_vol_thresh(volthresh);
//     optPriceDelta_ = EnumString::str_to_price_delta(priceDelta);
//     optDHL_ = EnumString::str_to_daily_hl(opt_dhl);
//     optLHL_ = EnumString::str_to_local_hl(opt_lhl);
//     underlyingPriceDelta_ = EnumString::str_to_price_delta(underlyingPriceDelta);
//     underlyingDHL_ = EnumString::str_to_daily_hl(u_dhl);
//     underlyingLHL_ = EnumString::str_to_local_hl(u_lhl);
// }

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