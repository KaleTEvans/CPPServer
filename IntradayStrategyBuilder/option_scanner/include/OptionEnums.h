#ifndef OPTIONENUMS_H
#define OPTIONENUMS_H

#include <iostream>
#include <unordered_map>

// The following enum classes will be used to designate tags for each option data element collected
// These tags will then be used to analyze correlations between success of each flagged option instance

namespace isb_option_tags {

// Option Types
enum OptionType {
	Call = 1,
	Put = 2
};

// Time frame for each different kind of candle created
enum TimeFrame {
	FiveSecs = 3,
	ThirtySecs = 4,
	OneMin = 5,
	FiveMin = 6,
	ThirtyMin = 53,
	OneHour = 54
};

// Option Strike relative to underlying price
enum RelativeToMoney {
	ATM = 7,
	OTM1 = 8, 
	OTM2 = 9, 
	OTM3 = 10, 
	OTM4 = 11, 
	OTM5 = 12,
	ITM1 = 13, 
	ITM2 = 14, 
	ITM3 = 15, 
	ITM4 = 16, 
	ITM5 = 17
};

// Time of day tagged by hour during stock market operating times
enum TimeOfDay { 
	Hour1 = 18, 
	Hour2 = 19, 
	Hour3 = 20, 
	Hour4 = 21, 
	Hour5 = 22, 
	Hour6 = 23, 
	Hour7 = 24 
};

// Volume Data
enum class VolumeStDev { 
	Over1 = 25, 
	Over2 = 26, 
	Over3 = 27, 
	Over4 = 28, 
	LowVol = 29 
};
enum class VolumeThreshold { 
	Vol100 = 30, 
	Vol250 = 31, 
	Vol500 = 32, 
	Vol1000 = 33, 
	LowVol = 34 
};

// Standard deviation of candle high to low
enum class PriceDelta { 
	Under1, 
	Under2, 
	Over2 
}; // Int values must be retrieved from tag_to_int function

// Whether contract or underlying are near intraday highs or lows
enum class DailyHighsAndLows { 
	NDL , 
	NDH, 
	Inside 
}; // Int values must be retrieved from tag_to_int function

// Whether contract or underlying are near local (30 min) highs or lows
enum class LocalHighsAndLows { 
	NLL, 
	NLH, 
	Inside 
}; // Int values must be retrieved from tag_to_int function

// Multi-candle rate of change categories
// Volume movement over several candles
enum VolumeROC {
	VolumeIncrease = 55, // Volume increase over 3 candles or more
	HighVolumeIncrease = 56, // Volume over standard deviation for 3 or more candles
	VolumeDecrease = 57 // Volume decrease over 3 candles or more
};

// Price movement over several candles
enum PriceROC {
	PriceIncrease = 58, // Price increase over 3 candles or more
	HighPriceIncrease = 59, // Price increase over stdev 3 or more candles
	PriceDecrease = 60, // 3 or more candles
	HighPriceDecrease = 61 // Decrease over stdev 3 or more candles
};

// Current intraday trending direction of the underlying
enum class TrendingDirectionIntraday {
	Bullish = 62,
	Bearish = 63,
	Sideways = 64
};

// Will determine how many days this trend will include, likely will be arbitrary
enum class TrendingDirectionDaily {
	Bullish = 65,
	Bearish = 66,
	Sideways = 67
};

// Categories for tags
enum TagCategory {
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
int tag_to_db_key(PriceDelta val, TagCategory tc);
int tag_to_db_key(DailyHighsAndLows val, TagCategory tc);
int tag_to_db_key(LocalHighsAndLows val, TagCategory tc);
}

#endif