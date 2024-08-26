#include "UnderlyingData.h"

void UnderlyingOneMinuteData::addFiveSecData(std::vector<std::shared_ptr<Candle>> fiveSecData)  {
    for (int i=0; i < fiveSecData.size(); i++) {
        if (i == 0) {
            open = fiveSecData[i]->open();
        }
        low = std::min(low, fiveSecData[i]->low());
        high = std::max(high, fiveSecData[i]->high());
    }
    close = fiveSecData.back()->close();
}

UnderlyingData::UnderlyingData(std::shared_ptr<tWrapper> wrapper, std::shared_ptr<CSVFileSaver> csv, Contract contract):
    wrapper(wrapper), csv(csv), contract(contract) {
        // Begin by sending out appropriate requests to generate data id's
        std::string tickRequests = "100,101,104,106,162,165,293,294,411,588";
        mktDataId = wrapper->reqMktData(contract, tickRequests, false, false);
        rtbId = wrapper->reqRealTimeBars(contract, 5, "TRADES", true);

        // Now set up the callback functions 
        wrapper->getMessageBus()->subscribe(EventType::TickPriceInfo, [this](std::shared_ptr<DataEvent> event) {
            // Use dynamic_pointer_cast to cast Event type to child class type
            if (event->getReqId() == this->mktDataId) this->handleTickPriceEvent(std::dynamic_pointer_cast<TickPriceEvent>(event));
        });

        wrapper->getMessageBus()->subscribe(EventType::TickSizeInfo, [this](std::shared_ptr<DataEvent> event) {
            if (event->getReqId() == this->mktDataId) this->handleTickSizeEvent(std::dynamic_pointer_cast<TickSizeEvent>(event));
        });

        wrapper->getMessageBus()->subscribe(EventType::TickGenericInfo, [this](std::shared_ptr<DataEvent> event) {
            if (event->getReqId() == this->mktDataId) this->handleTickGenericEvent(std::dynamic_pointer_cast<TickGenericEvent>(event));
        });

        wrapper->getMessageBus()->subscribe(EventType::TickNewsInfo, [this](std::shared_ptr<DataEvent> event) {
            this->handleTickNewsEvent(std::dynamic_pointer_cast<TickNewsEvent>(event));
        });

        wrapper->getMessageBus()->subscribe(EventType::RealTimeCandleData, [this](std::shared_ptr<DataEvent> event) {
            if (event->getReqId() == this->rtbId) this->handleRealTimeCandles(std::dynamic_pointer_cast<CandleDataEvent>(event));
        });

        // Send the request to get the options chain
        int chainReqId = requestOptionsChain();
        while (!wrapper->checkEventCompleted(chainReqId)) std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // Wait to get the first price tick
        while (currentPrice <= 1) std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // Now we should have all the data we need to send back to the option scanner
    }

int UnderlyingData::requestOptionsChain() {
    std::string underlyingSymbol = contract.symbol;
    std::string futFopExchange = contract.exchange;
    std::string underlyingSecType = contract.secType;
    int underlyingConId = contract.conId;
    
    int reqId = 0;
    wrapper->reqSecDefOptParams(
        [this](int reqId) {
            reqId = reqId;
        },
        [this](const std::string& exchange, int underlyingConId, const std::string& tradingClass, 
            const std::string& multiplier, const std::set<std::string>& expirations, 
            const std::set<double>& strikes) {
                this->handleOptionsChainData(exchange, underlyingConId, tradingClass, multiplier,
                    expirations, strikes);
            }, 
        underlyingSymbol, futFopExchange, underlyingSecType, underlyingConId
    );
    return reqId;
}

int UnderlyingData::getStrikeIncrement() { return strikeIncrement; }

std::pair<std::vector<double>, std::vector<double>> UnderlyingData::getStrikes(int countITM, double price) {
    int firstITM = 0;
    for (int i=0; i < optionsChain.size(); i++) {
        if (price > optionsChain[i]) {
            firstITM = i;
            break;
        }
    }

    std::vector<double> callStrikes;
    std::vector<double> putStrikes;
    int callIndex = firstITM - (countITM - 1);
    int putIndex = firstITM + countITM;
    while (callStrikes.size() != 10 && callIndex < optionsChain.size()) {
        callStrikes.push_back(optionsChain[callIndex]);
        callIndex++;
    }
    while (putStrikes.size() != 10 && putIndex >= 0) {
        putStrikes.push_back(optionsChain[putIndex]);
        putIndex--;
    }

    return {callStrikes, putStrikes};
}

void UnderlyingData::handleOptionsChainData(const std::string& exchange, 
    int underlyingConId, const std::string& tradingClass, const std::string& multiplier, 
    const std::set<std::string>& expirations, const std::set<double>& strikes) {
        // Clear any previous option chain data
        optionsChain.clear();
        for (auto& i : strikes) optionsChain.push_back(i);
        std::sort(optionsChain.begin(), optionsChain.end());

        // Check in middle to get strike increment in case it increases further in the money
        int mid = optionsChain.size() / 2;
        strikeIncrement = optionsChain[mid] - optionsChain[mid-1];
    }

void UnderlyingData::handleTickPriceEvent(std::shared_ptr<TickPriceEvent> event) {
    if (event->tickType == TickType::LAST) {
        // Check news tick map for time
        currentPrice = event->price;
        auto it = newsTicks.find(event->timeStamp);
        if (it == newsTicks.end()) {
            std::pair<std::shared_ptr<TickNewsEvent>, double> lastTick{nullptr, event->price};
            newsTicks.insert({event->timeStamp, lastTick});
        } else {
            it->second.second = event->price;
        }
    } else {
        switch (event->tickType)
        {
        case TickType::LOW_13_WEEK:
            low13Week = event->price;
            break;
        case TickType::HIGH_13_WEEK:
            high13Week = event->price;
            break;
        case TickType::LOW_26_WEEK:
            low26week = event->price;
            break;
        case TickType::HIGH_26_WEEK:
            high26Week = event->price;
            break;
        case TickType::LOW_52_WEEK:
            low52Week = event->price;
            break;
        case TickType::HIGH_52_WEEK:
            high52Week = event->price;
            break;
        case TickType::HIGH:
            oneMinuteData.back().dailyHigh = event->price;
            break;
        case TickType::LOW:
            oneMinuteData.back().dailyLow = event->price;
            break;
        
        default:
            std::cout << "Tick Price type not used" << std::endl;
            break;
        }
    }
}

void UnderlyingData::handleTickSizeEvent(std::shared_ptr<TickSizeEvent> event) {
    switch (event->tickType)
    {
    case TickType::AVG_VOLUME:
        averageVolume90Day = decimalToDouble(event->size);
        break;
    case TickType::OPTION_CALL_OPEN_INTEREST:
        callOpenInterest = decimalToDouble(event->size);
        break;
    case TickType::OPTION_PUT_OPEN_INTEREST:
        putOpenInterest = decimalToDouble(event->size);
        break;
    case TickType::OPTION_CALL_VOLUME:
        oneMinuteData.back().totalCallVolume = decimalToDouble(event->size);
        break;
    case TickType::OPTION_PUT_VOLUME:
        oneMinuteData.back().totalPutVolume = decimalToDouble(event->size);
        break;
    case TickType::VOLUME:
        oneMinuteData.back().dailyVolume = decimalToDouble(event->size);
        break;
    case TickType::FUTURES_OPEN_INTEREST:
        futuresOpenInterest = decimalToDouble(futuresOpenInterest);
        break;
    
    default:
        std::cout << "Tick Size type not used" << std::endl;
        break;
    }
}

void UnderlyingData::handleTickGenericEvent(std::shared_ptr<TickGenericEvent> event) {
    switch (event->tickType)
    {
    case TickType::INDEX_FUTURE_PREMIUM:
        oneMinuteData.back().indexFuturePremium = event->value;
        break;
    case TickType::TRADE_COUNT:
        oneMinuteData.back().totalTradeCount = event->value;
        break;
    case TickType::TRADE_RATE:
        oneMinuteData.back().oneMinuteTradeRate = event->value;
        break;
    case TickType::RT_HISTORICAL_VOL:
        oneMinuteData.back().realTimeHistoricalVolatility = event->value;
        break;
    
    default:
        std::cout << "Tick Generic type not used" << std::endl;
        break;
    }
}

void UnderlyingData::handleTickNewsEvent(std::shared_ptr<TickNewsEvent> event) {
    long timeStamp = event->dateTime;
    // Check news tick map for time
    auto it = newsTicks.find(timeStamp);
    if (it == newsTicks.end()) {
        std::pair<std::shared_ptr<TickNewsEvent>, double> lastTick{event, 0};
        newsTicks.insert({timeStamp, lastTick});
    } else {
        it->second.first = event;
    }
}

void UnderlyingData::handleRealTimeCandles(std::shared_ptr<CandleDataEvent> event) {
    fiveSecData.push_back(event->candle);

    // If there are 12 candles, add to one min candle and clear vector
    if (fiveSecData.size() >= 12) {
        oneMinuteData.back().addFiveSecData(fiveSecData);

        fiveSecData.clear();

        // Now add an empty one min candle to back of vector
        UnderlyingOneMinuteData oneMinCandle;
        oneMinuteData.push_back(oneMinCandle);
    }
}