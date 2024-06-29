#include "ContractData.h"

ContractData::ContractData(std::shared_ptr<tWrapper> wrapper, int mktDataId, int rtbId, Contract contract, double strikeIncrement) : 
    mktDataId(mktDataId), rtbId(rtbId), contract(contract), strikeIncrement(strikeIncrement), wrapper(wrapper) {
    // Subscribe to events
    wrapper->getMessageBus()->subscribe(EventType::TickPriceInfo, [this](std::shared_ptr<DataEvent> event) {
        // Use dynamic_pointer_cast to cast Event type to child class type
        if (event->getReqId() == this->mktDataId) this->handleTickPriceEvent(std::dynamic_pointer_cast<TickPriceEvent>(event));
    });

    wrapper->getMessageBus()->subscribe(EventType::TickGenericInfo, [this](std::shared_ptr<DataEvent> event) {
        if (event->getReqId() == this->mktDataId) this->handleTickGenericEvent(std::dynamic_pointer_cast<TickGenericEvent>(event));
    });

    wrapper->getMessageBus()->subscribe(EventType::TickSizeInfo, [this](std::shared_ptr<DataEvent> event) {
        if (event->getReqId() == this->mktDataId) this->handleTickSizeEvent(std::dynamic_pointer_cast<TickSizeEvent>(event));
    });

    wrapper->getMessageBus()->subscribe(EventType::TickStringInfo, [this](std::shared_ptr<DataEvent> event) {
        if (event->getReqId() == this->mktDataId) this->handleTickStringEvent(std::dynamic_pointer_cast<TickStringEvent>(event));
    });

    wrapper->getMessageBus()->subscribe(EventType::TickOptionInfo, [this](std::shared_ptr<DataEvent> event) {
        if (event->getReqId() == this->mktDataId) this->tickOptionInfo(std::dynamic_pointer_cast<TickOptionComputationEvent>(event));
    });

    wrapper->getMessageBus()->subscribe(EventType::TickNewsInfo, [this](std::shared_ptr<DataEvent> event) {
        this->tickNewsEvent(std::dynamic_pointer_cast<TickNewsEvent>(event));
    });

    wrapper->getMessageBus()->subscribe(EventType::RealTimeCandleData, [this](std::shared_ptr<DataEvent> event) {
        if (event->getReqId() == this->rtbId) this->realTimeCandles(std::dynamic_pointer_cast<CandleDataEvent>(event));
    });


    std::cout << "ContractData Request IDs: " << mktDataId << "," << rtbId << std::endl;
}

void ContractData::handleTickPriceEvent(std::shared_ptr<TickPriceEvent> event) {
    auto it = ticks.find(event->timeStamp);
    if (it == ticks.end()) {
        // Key does not exist, create a new MarketDataSingleFrame and insert it
        MarketDataSingleFrame mktData(event->timeStamp);
        mktData.tickPrice = event;
        ticks.insert({event->timeStamp, mktData});
    } else {
        // Key exists, update the existing MarketDataSingleFrame
        it->second.tickPrice = event;
    }
}

void ContractData::handleTickGenericEvent(std::shared_ptr<TickGenericEvent> event) {
    auto it = ticks.find(event->timeStamp);
    if (it == ticks.end()) {
        // Key does not exist, create a new MarketDataSingleFrame and insert it
        MarketDataSingleFrame mktData(event->timeStamp);
        mktData.tickGeneric = event;
        ticks.insert({event->timeStamp, mktData});
    } else {
        // Key exists, update the existing MarketDataSingleFrame
        it->second.tickGeneric = event;
    }
}

void ContractData::handleTickSizeEvent(std::shared_ptr<TickSizeEvent> event) {
    auto it = ticks.find(event->timeStamp);
    if (it == ticks.end()) {
        // Key does not exist, create a new MarketDataSingleFrame and insert it
        MarketDataSingleFrame mktData(event->timeStamp);
        mktData.tickSize = event;
        ticks.insert({event->timeStamp, mktData});
    } else {
        // Key exists, update the existing MarketDataSingleFrame
        it->second.tickSize = event;
    }
}

void ContractData::handleTickStringEvent(std::shared_ptr<TickStringEvent> event) {
    // First, parse the string element if it is a RT value
    if (event->tickType == TickType::RT_VOLUME) {
        std::shared_ptr<TimeAndSales> tas = std::make_shared<TimeAndSales>(event->value);

        auto it = ticks.find(event->timeStamp);
        if (it == ticks.end()) {
            // Key does not exist, create a new MarketDataSingleFrame and insert it
            MarketDataSingleFrame mktData(event->timeStamp);
            mktData.tickString = event;
            mktData.timeAndSales = tas;
            ticks.insert({event->timeStamp, mktData});
        } else {
            // Key exists, update the existing MarketDataSingleFrame
            it->second.tickString = event;
            it->second.timeAndSales = tas;
        }
    }
}

TimeAndSales::TimeAndSales(std::string data) : data(data) {
    // Replace semicolons with spaces for easier parsing using stringstream
    std::replace(data.begin(), data.end(), ';', ' ');

    // Using istringstream to parse the string
    std::istringstream iss(data);

    std::string priceStr;
    std::string quantityStr;
    std::string timeValueStr;
    std::string totalVolStr;
    std::string vwapStr;
    std::string mmStr;

    // Parsing the string into variables
    iss >> priceStr;
    iss >> quantityStr;
    iss >> timeValueStr;
    iss >> totalVolStr;
    iss >> vwapStr;
    iss >> mmStr;

    price = std::stod(priceStr);
    quantity = std::stod(quantityStr);
    timeValue = std::stol(timeValueStr);
    totalVol = std::stod(totalVolStr);
    vwap = stod(vwapStr);

    if (mmStr == "true") filledBySingleMM = true;
    else filledBySingleMM = false;

    // Rounding the decimal to 3 places
    vwap = std::round(vwap * 1000) / 1000;
}

void TimeAndSales::print() {
    std::cout << timeValue << " | Price: " << price << " | Quantity: " << quantity << " | Total Vol: "
        << totalVol << " | VWAP: " << vwap << " | Filled by single MM: " << filledBySingleMM << std::endl;
}

void ContractData::tickOptionInfo(std::shared_ptr<TickOptionComputationEvent> event) {
    lastUnderlyingPrice = event->undPrice;

    auto it = ticks.find(event->timeStamp);
    if (it == ticks.end()) {
        // Key does not exist, create a new MarketDataSingleFrame and insert it
        MarketDataSingleFrame mktData(event->timeStamp);
        mktData.tickOption = event;
        ticks.insert({event->timeStamp, mktData});
    } else {
        // Key exists, update the existing MarketDataSingleFrame
        it->second.tickOption = event;
    }
}

void ContractData::tickNewsEvent(std::shared_ptr<TickNewsEvent> event) {
    newsTicks.push_back({event->dateTime, event});
}

void ContractData::realTimeCandles(std::shared_ptr<CandleDataEvent> event) {
    double priceDiff = contract.strike - lastUnderlyingPrice;
    double multiple = priceDiff / strikeIncrement;
    double strike = 0;

    if (multiple >= 0) strike = std::ceil(multiple);
    else strike = std::floor(multiple);

    // If a call, positive strike means ITM
    RelativeToMoney rtm = RelativeToMoney::NoValue;
    if (contract.right == "C") rtm = getRTM(strike * -1);
    else rtm = getRTM(strike);

    std::shared_ptr<FiveSecondData> fsd = std::make_shared<FiveSecondData>(event->candle, rtm);
    fiveSecData[event->candle->time()] = fsd;

    // Each time a five sec candle is created, run through ticks to drop into candles if any
    // If any ticks precede first candle, disregard
    int64_t firstCandleTime = fiveSecData.begin()->first; 

    // Creat a temporary vector to save the times of all the ticks added to a candle
    std::vector<int64_t> addedTickTimes;

    // Save most recent option data for one minute candles
    std::shared_ptr<TickOptionComputationEvent> optionInfo = nullptr;

    for (auto const& i : ticks) {
        int64_t tickTime = (i.first / 5000) * 5000;
        tickTime = tickTime / 1000; // Get units in seconds

        if (tickTime >= firstCandleTime) {
            auto it = fiveSecData.find(tickTime);
            if (it == fiveSecData.end()) {
                continue;
            } else {
                // Find five sec candle in map, then add MarketFrame to map of ticks in candle
                auto itr = it->second->ticks.find(i.first);
                if (itr == it->second->ticks.end()) it->second->ticks.insert({i.first, i.second});
                
                // Continuously update option info to get most recent
                if (i.second.tickOption) optionInfo = i.second.tickOption;
            }
        } 

        // Save time of tick for removal
        addedTickTimes.push_back(i.first);
    }
    
    // Next, we need to loop back through the ticks and remove them from the tick map to save space
    for (auto const& i : addedTickTimes) ticks.erase(i);

    // If candle count is a multiple of 12, create one minute candle
    if (fiveSecCandles.size() % 12 == 0) {
        // Create temp vector and send to constructor
        std::vector<std::shared_ptr<FiveSecondData>> temp;
        int n = fiveSecCandles.size();
        int start = 0;
        if (n - 12 > 0) start = n - 12;
        for (auto i = start; i < fiveSecCandles.size(); i++) temp.push_back(fiveSecCandles[i]);
        std::shared_ptr<OneMinuteData> c = std::make_shared<OneMinuteData>(temp, optionInfo);
        oneMinuteCandles.push_back(c);
    }
}

void ContractData::printData() {
    std::cout << "==============================================================" << std::endl;
    for (auto& i : fiveSecData) {
        i.second->candle->printCandle();
        std::cout << "--------------------------------------------------------------" << std::endl;
        for (auto& j : i.second->ticks) {
            j.second.printMktData();
        }
    }
}

FiveSecondData::FiveSecondData(std::shared_ptr<Candle> candle, RelativeToMoney rtm) :
    candle(candle), rtm(rtm) {
        // candle->printCandle();
        // std::cout << "Relative To Money: " << tag_to_db_key(rtm) << std::endl;
    }

OneMinuteData::OneMinuteData(std::vector<std::shared_ptr<FiveSecondData>> candles, std::shared_ptr<TickOptionComputationEvent> optionInfo) {
    int n = candles.size()-1;
    auto& currentCandle = *candles[n];
    rtm = currentCandle.rtm;

    auto reqId = candles[0]->candle->reqId();
    auto time = candles[0]->candle->time();

    impliedVol = optionInfo->impliedVol;
    delta = optionInfo->delta;
    optPrice = optionInfo->optPrice;
    gamma = optionInfo->gamma;
    vega = optionInfo->vega;
    theta = optionInfo->theta;
    undPrice = optionInfo->undPrice;

    // Now loop over 5 sec candles in vector to get olhc and vol
    int vol = 0;
    double high = 0;
    double low = 10000;
    double open = candles[0]->candle->open();
    double close = currentCandle.candle->close();

    for (auto i = 0; i < candles.size()-1; i++) {
        if (candles[i]->candle->high() >= high) high = candles[i]->candle->high();
        if (candles[i]->candle->low() <= low) low = candles[i]->candle->low();
        vol += candles[i]->candle->volume();

        // Fill tick news element
        for (auto& news : candles[i]->tickNews) {
            tickNews.push_back(news);
        }
    }

    candle = std::make_shared<Candle>(reqId, time, open, high, low, close, vol);
}

MarketDataSingleFrame::MarketDataSingleFrame(int64_t timestamp) : timestamp(timestamp) {}

void MarketDataSingleFrame::printMktData() {
    if (tickPrice != nullptr) {
        printf( "%lu | Tick Price. Ticker Id: %d, Field: %s, Price: %f, CanAutoExecute: %d," 
            "PastLimit: %d, PreOpen: %d\n", 
            tickPrice->timeStamp, tickPrice->reqId, *(TickTypes::ENUMS)tickPrice->tickType, tickPrice->price, 
            tickPrice->attrib.canAutoExecute, tickPrice->attrib.pastLimit, tickPrice->attrib.preOpen);
    }

    if (tickGeneric != nullptr) {
        printf( "%lu | Tick Generic. Ticker Id: %d, Type: %s, Value: %f\n", 
            tickGeneric->timeStamp, tickGeneric->reqId, *(TickTypes::ENUMS)tickGeneric->tickType, tickGeneric->value);
    }

    if (tickSize != nullptr) {
        printf( "%lu | Tick Size. Ticker Id: %d, Field: %s, Size: %s\n", 
            tickSize->timeStamp, tickSize->reqId, *(TickTypes::ENUMS)tickSize->tickType, 
            decimalStringToDisplay(tickSize->size).c_str());
    }

    if (tickString != nullptr) {
        printf("%lu | Tick String. Ticker Id: %d, Field: %s, Value: %s\n",
        tickString->timeStamp, tickString->reqId, *(TickTypes::ENUMS)tickString->tickType, tickString->value.c_str());
    
    }

    if (timeAndSales != nullptr) {
        timeAndSales->print();
    }

    if (tickOption != nullptr) {
        printf("%lu | TickOptionComputation. Ticker Id: %d, Type: %s, TickAttrib: %d," 
        "ImpliedVolatility: %f, Delta: %f, OptionPrice: %f, pvDividend: %f, Gamma: %f," 
        "Vega: %f, Theta: %f, Underlying Price: %f\n", tickOption->timeStamp, tickOption->reqId, 
        *(TickTypes::ENUMS)tickOption->tickType, tickOption->tickAttrib, tickOption->impliedVol, tickOption->delta, tickOption->optPrice, tickOption->pvDividend, 
        tickOption->gamma, tickOption->vega, tickOption->theta, tickOption->undPrice);
    }

    std::cout << "-------------------------------------------------------------------------" << std::endl;
}