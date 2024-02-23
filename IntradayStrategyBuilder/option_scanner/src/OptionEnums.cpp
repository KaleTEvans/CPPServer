#include "../include/OptionEnums.h"

namespace isb_option_tags {

int tag_to_db_key(PriceDelta val, TagCategory tc) {
    if (tc == TagCategory::UnderlyingPriceDelta) {
        switch (val)
        {
        case PriceDelta::Under1: return 35;
        case PriceDelta::Under2: return 36;
        case PriceDelta::Over2: return 37;
        default: return 0;
        }
    } else if (tc == TagCategory::OptionPriceDelta) {
        switch (val)
        {
        case PriceDelta::Under1: return 38;
        case PriceDelta::Under2: return 39;
        case PriceDelta::Over2: return 40;
        default: return 0;
        }
    } else {
        std::cout << "Enum Error: Invalid TagCategory and PriceDelta" << std::endl;
        return 0;
    }
}

int tag_to_db_key(DailyHighsAndLows val, TagCategory tc) {
    if (tc == TagCategory::UnderlyingDailyHighsAndLows) {
        switch (val)
        {
        case DailyHighsAndLows::NDL: return 41;
        case DailyHighsAndLows::NDH: return 42;
        case DailyHighsAndLows::Inside: return 43;
        default: return 0;
        }
    } else if (tc == TagCategory::OptionDailyHighsAndLows) {
         switch (val)
        {
        case DailyHighsAndLows::NDL: return 44;
        case DailyHighsAndLows::NDH: return 45;
        case DailyHighsAndLows::Inside: return 46;
        default: return 0;
        }
    } else {
        std::cout << "Enum Error: Invalid TagCategory and DHL" << std::endl;
        return 0;
    }
}

int tag_to_db_key(LocalHighsAndLows val, TagCategory tc) {
    if (tc == TagCategory::UnderlyingLocalHighsAndLows) {
        switch (val)
        {
        case LocalHighsAndLows::NLL: return 47;
        case LocalHighsAndLows::NLH: return 48;
        case LocalHighsAndLows::Inside: return 49;
        default: return 0;
        }
    } else if (tc == TagCategory::OptionLocalHighsAndLows) {
        switch (val)
        {
        case LocalHighsAndLows::NLL: return 50;
        case LocalHighsAndLows::NLH: return 51;
        case LocalHighsAndLows::Inside: return 52;
        default: return 0;
        }
    } else {
        std::cout << "Enum Error: Invalid TagCategory and LHL" << std::endl;
        return 0;
    }
}

std::string tag_category(TagCategory val) {
	switch (val)
	{
	case OptionType: return "OptionType";
	case TimeFrame: return "TimeFrame";
	case RelativeToMoney: return "RelativeToMoney";
	case VolumeStDev: return "VolumeStDev";
	case VolumeThreshold: return "VolumeThreshold";
	case UnderlyingPriceDelta: return "UnderlyingPriceDelta";
	case OptionPriceDelta: return "OptionPriceDelta";
	case UnderlyingDailyHighsAndLows: return "UnderlyingDailyHighsAndLows";
	case OptionDailyHighsAndLows: return "OptionDailyHighsAndLows";
	case UnderlyingLocalHighsAndLows: return "UnderlyingLocalHighsAndLows";
	case OptionLocalHighsAndLows: return "OptionLocalHighsAndLows";
    case MultiCandleROC: return "MultiCandleRateOfChange";
    case TrendingDirection: return "TrendingDirection";
	default: return "";
	}
}


}