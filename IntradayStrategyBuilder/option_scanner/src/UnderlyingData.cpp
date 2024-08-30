#include "UnderlyingData.h"

void UnderlyingOneMinuteData::addFiveSecData(std::vector<std::shared_ptr<Candle>> fiveSecData)  {
    for (int i=0; i < fiveSecData.size(); i++) {
        if (i == 0) {
            time = fiveSecData[i]->time();
            open = fiveSecData[i]->open();
        }
        low = std::min(low, fiveSecData[i]->low());
        high = std::max(high, fiveSecData[i]->high());
        volume += fiveSecData[i]->volume();
    }
    close = fiveSecData.back()->close();
}

std::string UnderlyingOneMinuteData::formatCSV() {
    return std::to_string(time) + "," +
            CSVFileSaver::valueToCSV(open) + "," +
            CSVFileSaver::valueToCSV(high) + "," +
            CSVFileSaver::valueToCSV(close) + "," +
            CSVFileSaver::valueToCSV(volume) + "," +
            CSVFileSaver::valueToCSV(dailyHigh) + "," +
            CSVFileSaver::valueToCSV(dailyLow) + "," +
            CSVFileSaver::valueToCSV(dailyVolume) + "," +
            CSVFileSaver::valueToCSV(totalCallVolume) + "," +
            CSVFileSaver::valueToCSV(totalPutVolume) + "," +
            CSVFileSaver::valueToCSV(indexFuturePremium) + "," +
            CSVFileSaver::valueToCSV(totalTradeCount) + "," +
            CSVFileSaver::valueToCSV(oneMinuteTradeRate) + "," +
            CSVFileSaver::valueToCSV(realTimeHistoricalVolatility) + "," +
            CSVFileSaver::valueToCSV(optionImpliedVolatility) + "," +
            CSVFileSaver::valueToCSV(callOpenInterest) + "," +
            CSVFileSaver::valueToCSV(putOpenInterest) + "," +
            CSVFileSaver::valueToCSV(futuresOpenInterest) + "\n";
}

std::string ContractNewsData::formatCSV() {
    return std::to_string(time) + "," +
            articleId + "," +
            headline + "," +
            CSVFileSaver::valueToCSV(sentimentScore) + "," +
            CSVFileSaver::valueToCSV(price) + "\n";
}

UnderlyingData::UnderlyingData(std::shared_ptr<tWrapper> wrapper, std::shared_ptr<CSVFileSaver> csv, Contract contract):
    wrapper(wrapper), csv(csv), contract(contract) {
        // Create files for filesaver
        csv->createDirectoriesAndFiles(contract.symbol, 0, "None");
        // Initialize empty one minute candle prior to receiving data
        UnderlyingOneMinuteData um;
        oneMinuteData.push_back(um);
        // Begin by sending out appropriate requests to generate data id's
        // Only some requests will work for a stock vs index type
        std::string tickRequests;
        if (contract.secType == "IND") {
            tickRequests = "100,101,104,106,162,165,293,294,411,588";
        } else {
            tickRequests = "100,101,106,165,293,294,411,588";
        }
        mktDataId = wrapper->reqMktData(contract, tickRequests, false, false);
        rtbId = wrapper->reqRealTimeBars(contract, 5, "TRADES", true);
        // Create news request
        Contract news = ContractDefs::BZBroadTape();
        wrapper->reqMktData(news, "mdoff,292", false, false);

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

        // Wait to get the first price tick
        while (currentPrice <= 1) std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // Now we should have all the data we need to send back to the option scanner
        // Send the request to get the options chain
        int chainReqId = requestOptionsChain();
        while (!wrapper->checkEventCompleted(chainReqId)) std::this_thread::sleep_for(std::chrono::milliseconds(10));
        getStrikes(2, currentPrice);
    }

std::string UnderlyingData::formatAveragesCSV() {
    return CSVFileSaver::valueToCSV(low13Week) + "," + 
            CSVFileSaver::valueToCSV(high13Week) + "," +
            CSVFileSaver::valueToCSV(low26week) + "," +
            CSVFileSaver::valueToCSV(high26Week) + "," +
            CSVFileSaver::valueToCSV(low52Week) + "," +
            CSVFileSaver::valueToCSV(high52Week) + "," +
            CSVFileSaver::valueToCSV(averageVolume90Day) + "\n";
}

int UnderlyingData::requestOptionsChain() {
    std::string underlyingSymbol = contract.symbol;
    std::string futFopExchange = "";
    std::string underlyingSecType = contract.secType;
    int underlyingConId = ContractDefs::getConId(contract.symbol);
    
    int optId = 0;
    wrapper->reqSecDefOptParams(
        [this, &optId](int reqId) {
            optId = reqId;
        },
        [this](const std::string& exchange, int underlyingConId, const std::string& tradingClass, 
            const std::string& multiplier, const std::set<std::string>& expirations, 
            const std::set<double>& strikes) {
                this->handleOptionsChainData(exchange, underlyingConId, tradingClass, multiplier,
                    expirations, strikes);
            }, 
        underlyingSymbol, futFopExchange, underlyingSecType, underlyingConId
    );
    return optId;
}

int UnderlyingData::getStrikeIncrement() { return strikeIncrement; }

std::pair<std::vector<double>, std::vector<double>> UnderlyingData::getStrikes(int countITM, double price) {
    // Insert set items into vector for easier manipulation
    for (const auto& i : optionStrikes) optionsChain.push_back(i);
    // Get midpoint and find strike increment
    int mid = optionsChain.size() / 2;
    strikeIncrement = optionsChain[mid+1] - optionsChain[mid];
    std::cout << "Strike Increment: " << strikeIncrement << std::endl; 
    
    // Loop until the first strike out of the money (for calls)
    int firstOTM = 0;
    for (int i=0; i < optionsChain.size(); i++) {
        if (price < optionsChain[i]) {
            firstOTM = i;
            break;
        }
    }

    std::vector<double> callStrikes;
    std::vector<double> putStrikes;
    int callIndex = firstOTM - countITM;
    int putIndex = firstOTM + (countITM - 1);
    while (callStrikes.size() != 10 && callIndex < optionsChain.size()) {
        callStrikes.push_back(optionsChain[callIndex]);
        callIndex++;
    }
    while (putStrikes.size() != 10 && putIndex >= 0) {
        putStrikes.push_back(optionsChain[putIndex]);
        putIndex--;
    }

    std::cout << "Call Strikes: " << std::endl;
    for (auto& i: callStrikes) std::cout << i << " ";
    std::cout << std::endl;
    std::cout << "Put Strikes: " << std::endl;
    for (auto& i: putStrikes) std::cout << i << " ";
    std::cout << std::endl; 

    return {callStrikes, putStrikes};
}

void UnderlyingData::handleOptionsChainData(const std::string& exchange, 
    int underlyingConId, const std::string& tradingClass, const std::string& multiplier, 
    const std::set<std::string>& expirations, const std::set<double>& strikes) {
        // Put items in class member set to prevent duplicates from multiple exchanges
        for (auto& i : strikes) {
            if (optionStrikes.find(i) == optionStrikes.end()) {
                optionStrikes.insert(i);
            }
        }
    }

void UnderlyingData::handleTickPriceEvent(std::shared_ptr<TickPriceEvent> event) {
    if (event->tickType == TickType::LAST) {
        // Check news tick map for time
        currentPrice = event->price;
        auto it = newsTicks.find(event->timeStamp);
        // If the time isn't in the map already, insert empty news object to track price in the same file
        // If an object already exists, ignore it
        if (it == newsTicks.end()) {
            std::pair<std::shared_ptr<TickNewsEvent>, double> lastTick{nullptr, event->price};
            newsTicks.insert({event->timeStamp, lastTick});

            // Create news object with empty news columns and send to csv
            ContractNewsData cnd;
            cnd.time = event->timeStamp;
            cnd.price = currentPrice;
            csv->addDataToQueue(contract.symbol, 0, "None", DataType::News, cnd.formatCSV());
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

    event->print();
}

void UnderlyingData::handleTickSizeEvent(std::shared_ptr<TickSizeEvent> event) {
    switch (event->tickType)
    {
    case TickType::AVG_VOLUME:
        averageVolume90Day = decimalToDouble(event->size);
        break;
    case TickType::OPTION_CALL_OPEN_INTEREST:
        oneMinuteData.back().callOpenInterest = decimalToDouble(event->size);
        break;
    case TickType::OPTION_PUT_OPEN_INTEREST:
        oneMinuteData.back().putOpenInterest = decimalToDouble(event->size);
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
        oneMinuteData.back().futuresOpenInterest = decimalToDouble(event->size);
        break;
    
    default:
        std::cout << "Tick Size type not used" << std::endl;
        break;
    }

    event->print();
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
    case TickType::OPTION_HISTORICAL_VOL:
        oneMinuteData.back().optionHistoricalVolatility = event->value;
        break;
    case TickType::OPTION_IMPLIED_VOL:
        oneMinuteData.back().optionImpliedVolatility = event->value;
        break;
    
    default:
        std::cout << "Tick Generic type not used" << std::endl;
        break;
    }

    event->print();
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

    // Since there might be multiple news articles at the same time, we will just ignore the map
    // and create objects for each event to save in the data table
    ContractNewsData cnd;
    cnd.time = timeStamp;
    cnd.articleId = event->articleId;
    cnd.headline = event->headline;
    cnd.sentimentScore = event->sentimentScore;
    cnd.price = currentPrice;
    csv->addDataToQueue(contract.symbol, 0, "None", DataType::News, cnd.formatCSV());

    event->print();
}

void UnderlyingData::handleRealTimeCandles(std::shared_ptr<CandleDataEvent> event) {
    fiveSecData.push_back(event->candle);

    // If there are 12 candles, add to one min candle and clear vector
    if (fiveSecData.size() >= 12) {
        oneMinuteData.back().addFiveSecData(fiveSecData);

        fiveSecData.clear();

        // Save new candle data
        csv->addDataToQueue(contract.symbol, 0, "None", DataType::UnderlyingOneMinute, oneMinuteData.back().formatCSV());

        // Now add an empty one min candle to back of vector
        UnderlyingOneMinuteData oneMinCandle;
        oneMinuteData.push_back(oneMinCandle);
    }

    event->candle->printCandle();
}