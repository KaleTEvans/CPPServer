#include "OptionEnums.h"

namespace isb_option_tags {

int tag_to_db_key(OptionType val) {
    switch (val)
    {
    case OptionType::Call: return 1;
    case OptionType::Put: return 2;
    default: return 0;
    }
}

int tag_to_db_key(TimeFrame val) {
    switch (val)
    {
    case TimeFrame::FiveSecs: return 3;
    case TimeFrame::ThirtySecs: return 4;
    case TimeFrame::OneMin: return 5;
    case TimeFrame::FiveMin: return 6;
    case TimeFrame::ThirtyMin: return 53;
    case TimeFrame::OneHour: return 54;
    default: return 0;
    }
}

int tag_to_db_key(RelativeToMoney val) {
    switch (val)
    {
    case RelativeToMoney::ATM: return 7;
    case RelativeToMoney::OTM1: return 8;
    case RelativeToMoney::OTM2: return 9;
    case RelativeToMoney::OTM3: return 10;
    case RelativeToMoney::OTM4: return 11;
    case RelativeToMoney::OTM5: return 12;
    case RelativeToMoney::ITM1: return 13;
    case RelativeToMoney::ITM2: return 14;
    case RelativeToMoney::ITM3: return 15;
    case RelativeToMoney::ITM4: return 16;
    case RelativeToMoney::ITM5: return 17;
    default: return 0;
    }
}

RelativeToMoney getRTM(int x) {
    switch (x)
    {
    case 0: return RelativeToMoney::ATM;
    case 1: return RelativeToMoney::ITM1;
    case 2: return RelativeToMoney::ITM2;
    case 3: return RelativeToMoney::ITM3;
    case 4: return RelativeToMoney::ITM4;
    case 5: return RelativeToMoney::ITM5;
    case 6: return RelativeToMoney::ITM6;
    
    case -1: return RelativeToMoney::OTM1;
    case -2: return RelativeToMoney::OTM2;
    case -3: return RelativeToMoney::OTM3;
    case -4: return RelativeToMoney::OTM4;
    case -5: return RelativeToMoney::OTM5;
    case -6: return RelativeToMoney::OTM6;
    case -7: return RelativeToMoney::OTM7;
    case -8: return RelativeToMoney::OTM8;
    case -9: return RelativeToMoney::OTM9;
    case -10: return RelativeToMoney::OTM10;
    case -11: return RelativeToMoney::OTM11;
    case -12: return RelativeToMoney::OTM12;
    
    default: return RelativeToMoney::NoValue;
    }
}

std::string getRTMstr(RelativeToMoney rtm) {
    switch (rtm)
    {
    case RelativeToMoney::ATM: return "ATM";
    case RelativeToMoney::ITM1: return "ITM1";
    case RelativeToMoney::ITM2: return "ITM2";
    case RelativeToMoney::ITM3: return "ITM3";
    case RelativeToMoney::ITM4: return "ITM4";
    case RelativeToMoney::ITM5: return "ITM5";
    case RelativeToMoney::ITM6: return "ITM6";
    
    case RelativeToMoney::OTM1: return "OTM1";
    case RelativeToMoney::OTM2: return "OTM2";
    case RelativeToMoney::OTM3: return "OTM3";
    case RelativeToMoney::OTM4: return "OTM4";
    case RelativeToMoney::OTM5: return "OTM5";
    case RelativeToMoney::OTM6: return "OTM6";
    case RelativeToMoney::OTM7: return "OTM7";
    case RelativeToMoney::OTM8: return "OTM8";
    case RelativeToMoney::OTM9: return "OTM9";
    case RelativeToMoney::OTM10: return "OTM10";
    case RelativeToMoney::OTM11: return "OTM11";
    case RelativeToMoney::OTM12: return "OTM12";

    case RelativeToMoney::NoValue: return "NoValue";
    
    default: return "NoValue";
    }
}

int tag_to_db_key(TimeOfDay val) {
    switch (val)
    {
    case TimeOfDay::Hour1: return 18;
    case TimeOfDay::Hour2: return 19;
    case TimeOfDay::Hour3: return 20;
    case TimeOfDay::Hour4: return 21;
    case TimeOfDay::Hour5: return 22;
    case TimeOfDay::Hour6: return 23;
    case TimeOfDay::Hour7: return 24;
    default: return 0;
    }
}

int tag_to_db_key(VolumeStDev val) {
    switch (val)
    {
    case VolumeStDev::Over1: return 25;
    case VolumeStDev::Over2: return 26;
    case VolumeStDev::Over3: return 27;
    case VolumeStDev::Over4: return 28;
    case VolumeStDev::LowVol: return 29;
    default: return 0;
    }
}

int tag_to_db_key(VolumeThreshold val) {
    switch (val) 
    {
    case VolumeThreshold::Vol100: return 30;
    case VolumeThreshold::Vol250: return 31;
    case VolumeThreshold::Vol500: return 32;
    case VolumeThreshold::Vol1000: return 33;
    case VolumeThreshold::LowVol: return 34;
    default: return 0;
    }
}

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

int tag_to_db_key(VolumeROC val) {
    switch (val)
    {
    case VolumeROC::VolumeIncrease: return 55;
    case VolumeROC::HighVolumeIncrease: return 56;
    case VolumeROC::VolumeDecrease: return 57;
    default: return 0;
    }
}

int tag_to_db_key(PriceROC val) {
    switch (val)
    {
    case PriceROC::PriceIncrease: return 58;
    case PriceROC::HighPriceIncrease: return 59;
    case PriceROC::PriceDecrease: return 60;
    case PriceROC::HighPriceDecrease: return 61;
    default: return 0;
    }
}

int tag_to_db_key(TrendingDirectionIntraday val) {
    switch (val)
    {
    case TrendingDirectionIntraday::Bullish: return 62;
    case TrendingDirectionIntraday::Bearish: return 63;
    case TrendingDirectionIntraday::Sideways: return 64;
    default: return 0;
    }
}

int tag_to_db_key(TrendingDirectionDaily val) {
    switch (val)
    {
    case TrendingDirectionDaily::Bullish: return 65;
    case TrendingDirectionDaily::Bearish: return 66;
    case TrendingDirectionDaily::Sideways: return 67;
    default: return 0;
    }
}

std::string tag_category(TagCategory val) {
	switch (val)
	{
	case TagCategory::OptionType: return "OptionType";
	case TagCategory::TimeFrame: return "TimeFrame";
	case TagCategory::RelativeToMoney: return "RelativeToMoney";
	case TagCategory::VolumeStDev: return "VolumeStDev";
	case TagCategory::VolumeThreshold: return "VolumeThreshold";
	case TagCategory::UnderlyingPriceDelta: return "UnderlyingPriceDelta";
	case TagCategory::OptionPriceDelta: return "OptionPriceDelta";
	case TagCategory::UnderlyingDailyHighsAndLows: return "UnderlyingDailyHighsAndLows";
	case TagCategory::OptionDailyHighsAndLows: return "OptionDailyHighsAndLows";
	case TagCategory::UnderlyingLocalHighsAndLows: return "UnderlyingLocalHighsAndLows";
	case TagCategory::OptionLocalHighsAndLows: return "OptionLocalHighsAndLows";
    case TagCategory::MultiCandleROC: return "MultiCandleRateOfChange";
    case TagCategory::TrendingDirection: return "TrendingDirection";
	default: return "";
	}
}


}