#pragma once

#include <iostream>
#include <unordered_map>

// The following enum classes will be used to designate tags for each option data element collected
// These tags will then be used to analyze correlations between success of each flagged option instance

// Time frame for each different kind of candle created
enum class TimeFrame {
	FiveSecs,
	ThirtySecs,
	OneMin,
	FiveMin,
	ThirtyMin,
	OneHour
};

// Option Types
enum class OptionType {
	Call,
	Put
};

// Option Strike relative to underlying price
enum class RelativeToMoney {
	ATM,
	ITM1, ITM2, ITM3, ITM4, ITM5,
	OTM1, OTM2, OTM3, OTM4, OTM5
};

// Time of day tagged by hour during stock market operating times
enum class TimeOfDay { Hour1, Hour2, Hour3, Hour4, Hour5, Hour6, Hour7 };


