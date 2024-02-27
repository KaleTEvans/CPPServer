#ifndef OPTIONENUMS_H
#define OPTIONENUMS_H

#include <iostream>
#include <unordered_map>

// The following enum classes will be used to designate tags for each option data element collected
// These tags will then be used to analyze correlations between success of each flagged option instance

namespace isb_option_tags {

// Option Types
enum class OptionType {
	Call,
	Put,
	NoValue
};

// Time frame for each different kind of candle created
enum class TimeFrame {
	FiveSecs,
	ThirtySecs,
	OneMin,
	FiveMin,
	ThirtyMin,
	OneHour,
	NoValue
};

// Option Strike relative to underlying price
enum class RelativeToMoney {
	ATM,
	OTM1, 
	OTM2, 
	OTM3, 
	OTM4, 
	OTM5,
	ITM1, 
	ITM2, 
	ITM3, 
	ITM4, 
	ITM5,
	NoValue
};

// Time of day tagged by hour during stock market operating times
enum class TimeOfDay { 
	Hour1,
	Hour2,
	Hour3,
	Hour4,
	Hour5,
	Hour6,
	Hour7,
	NoValue
};

// Volume Data
enum class VolumeStDev { 
	Over1,
	Over2, 
	Over3, 
	Over4, 
	LowVol,
	NoValue
};
enum class VolumeThreshold { 
	Vol100, 
	Vol250, 
	Vol500, 
	Vol1000, 
	LowVol,
	NoValue 
};

// Standard deviation of candle high to low
enum class PriceDelta { 
	Under1, 
	Under2, 
	Over2,
	NoValue 
};

// Whether contract or underlying are near intraday highs or lows
enum class DailyHighsAndLows { 
	NDL , 
	NDH, 
	Inside,
	NoValue 
};

// Whether contract or underlying are near local (30 min) highs or lows
enum class LocalHighsAndLows { 
	NLL, 
	NLH, 
	Inside,
	NoValue 
}; 

// Multi-candle rate of change categories
// Volume movement over several candles
enum class VolumeROC {
	VolumeIncrease, // Volume increase over 3 candles or more
	HighVolumeIncrease, // Volume over standard deviation for 3 or more candles
	VolumeDecrease, // Volume decrease over 3 candles or more
	NoValue
};

// Price movement over several candles
enum class PriceROC {
	PriceIncrease, // Price increase over 3 candles or more
	HighPriceIncrease, // Price increase over stdev 3 or more candles
	PriceDecrease, // 3 or more candles
	HighPriceDecrease, // Decrease over stdev 3 or more candles
	NoValue
};

// Current intraday trending direction of the underlying
enum class TrendingDirectionIntraday {
	Bullish,
	Bearish,
	Sideways,
	NoValue
};

// Will determine how many days this trend will include, likely will be arbitrary
enum class TrendingDirectionDaily {
	Bullish,
	Bearish,
	Sideways,
	NoValue
};

// Categories for tags
enum class TagCategory {
	OptionType,
	TimeFrame,
	RelativeToMoney,
	TimeOfDay,
	VolumeStDev,
	VolumeThreshold,
	UnderlyingPriceDelta,
	OptionPriceDelta,
	UnderlyingDailyHighsAndLows,
	OptionDailyHighsAndLows,
	UnderlyingLocalHighsAndLows,
	OptionLocalHighsAndLows,
	MultiCandleROC,
	TrendingDirection
};

// String Conversions
std::string tag_category(TagCategory val);

// Tag to int key conversions
int tag_to_db_key(OptionType val);
int tag_to_db_key(TimeFrame val);
int tag_to_db_key(RelativeToMoney val);
int tag_to_db_key(TimeOfDay val);
int tag_to_db_key(VolumeStDev val);
int tag_to_db_key(VolumeThreshold val);
int tag_to_db_key(PriceDelta val, TagCategory tc);
int tag_to_db_key(DailyHighsAndLows val, TagCategory tc);
int tag_to_db_key(LocalHighsAndLows val, TagCategory tc);
int tag_to_db_key(VolumeROC val);
int tag_to_db_key(PriceROC val);
int tag_to_db_key(TrendingDirectionIntraday val);
int tag_to_db_key(TrendingDirectionDaily val);
}

#endif