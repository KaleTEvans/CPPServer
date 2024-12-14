#include "UnderlyingData.h"

void UnderlyingOneMinuteData::addFiveSecData(std::vector<std::shared_ptr<Candle>> fiveSecData)  {
    for (int i=0; i < fiveSecData.size(); i++) {
        if (i == 0) {
            time = fiveSecData[i]->time();
            open = fiveSecData[i]->open();
        }
        low = std::min(low, fiveSecData[i]->low());
        high = std::max(high, fiveSecData[i]->high());
        volume += decimalToDouble(fiveSecData[i]->volume());
        close = fiveSecData[i]->close();
    }
}

std::string UnderlyingOneMinuteData::formatCSV() {
    return std::to_string(time) + "," +
            CSVFileSaver::valueToCSV(open) + "," +
            CSVFileSaver::valueToCSV(high) + "," +
            CSVFileSaver::valueToCSV(low) + "," +
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

std::string UnderlyingOneMinuteData::serializeOneMinData(const std::string& symbol) {
    Message message;
    message.set_type("underlying_contract");

    UnderlyingContract* underlyingContract = message.mutable_underlying_contract();
    underlyingContract->set_symbol(symbol);

    UnderlyingOneMinData* oneMin = underlyingContract->mutable_underlying_one_min()->Add();
    oneMin->set_time(time);
    oneMin->set_open(open);
    oneMin->set_high(high);
    oneMin->set_low(low);
    oneMin->set_close(close);
    if (volume != -1) oneMin->set_volume(volume);
    if (dailyHigh != -1) oneMin->set_daily_high(dailyHigh);
    if (dailyLow != -1) oneMin->set_daily_low(dailyLow);
    if (dailyVolume != -1) oneMin->set_daily_volume(dailyVolume);
    if (totalCallVolume != -1) oneMin->set_total_call_volume(totalCallVolume);
    if (totalPutVolume != -1) oneMin->set_total_put_volume(totalPutVolume);
    if (indexFuturePremium != -1) oneMin->set_index_future_premium(indexFuturePremium);
    if (totalTradeCount != -1) oneMin->set_total_trade_count(totalTradeCount);
    if (oneMinuteTradeRate != -1) oneMin->set_one_minute_trade_rate(oneMinuteTradeRate);
    if (realTimeHistoricalVolatility != -100) oneMin->set_real_time_historical_volatility(realTimeHistoricalVolatility);
    if (optionImpliedVolatility != -100) oneMin->set_option_implied_volatility(optionImpliedVolatility);
    if (callOpenInterest != -1) oneMin->set_call_open_interest(callOpenInterest);
    if (putOpenInterest != -1) oneMin->set_put_open_interest(putOpenInterest);
    if (futuresOpenInterest != -1) oneMin->set_futures_open_interest(futuresOpenInterest);

    std::string serialized;
    message.SerializeToString(&serialized);

    return serialized;
}

std::string ContractNewsData::formatCSV() {
    return std::to_string(time) + "," +
            articleId + "," +
            escapeCommas(headline) + "," +
            CSVFileSaver::valueToCSV(sentimentScore) + "," +
            CSVFileSaver::valueToCSV(price) + "\n";
}

std::string ContractNewsData::serializeNewsObject() {
    Message message;
    message.set_type("news");
    NewsEvent* news = message.mutable_news();
    news->set_time(time);
    news->set_article_id(articleId);
    news->set_headline(headline);
    news->set_sentiment_score(sentimentScore);

    std::string serialized;
    message.SerializeToString(&serialized);

    return serialized;
}

std::string ContractNewsData::escapeCommas(const std::string& value) {
    if (value.find(',') != std::string::npos) {
        return "\"" + value + "\"";
    }
    return value;
}

UnderlyingData::UnderlyingData(std::shared_ptr<tWrapper> wrapper, std::shared_ptr<SocketDataCollector> sdc, Contract contract):
    wrapper(wrapper), sdc(sdc), contract(contract) {
        // Create files for filesaver
        //csv->createDirectoriesAndFiles(contract.symbol, 0, "None");
        // Initialize empty one minute candle prior to receiving data
        UnderlyingOneMinuteData um;
        oneMinuteData.push_back(um);

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

        // wrapper->getMessageBus()->subscribe(EventType::TickNewsInfo, [this](std::shared_ptr<DataEvent> event) {
        //     this->handleTickNewsEvent(std::dynamic_pointer_cast<TickNewsEvent>(event));
        // });

        wrapper->getMessageBus()->subscribe(EventType::RealTimeCandleData, [this](std::shared_ptr<DataEvent> event) {
            if (event->getReqId() == this->rtbId) this->handleRealTimeCandles(std::dynamic_pointer_cast<CandleDataEvent>(event));
        });
    }

UnderlyingData::~UnderlyingData() {
    //stopReceivingData();
}

Contract UnderlyingData::getContract() { return contract; }

void UnderlyingData::startReceivingData() {
    // Begin by sending out appropriate requests to generate data id's
    // Only some requests will work for a stock vs index type
    std::string tickRequests;
    if (contract.secType == "IND") {
        tickRequests = "100,101,104,106,162,165,293,294,411,588";
    } else {
        tickRequests = "100,101,106,165,293,294,411,588";
    }

    std::cout << "Data request sent for underlying contract: " << contract.symbol << std::endl;

    mktDataId = wrapper->reqMktData(contract, tickRequests, false, false);
    rtbId = wrapper->reqRealTimeBars(contract, 5, "TRADES", true);
    // Create news request
    Contract news = ContractDefs::BZBroadTape();
    wrapper->reqMktData(news, "mdoff,292", false, false);

    // Wait to get the first price tick
    while (currentPrice <= 1) std::this_thread::sleep_for(std::chrono::milliseconds(10));
    // Now we should have all the data we need to send back to the option scanner
    // Send the request to get the options chain
    int chainReqId = requestOptionsChain();
    while (!wrapper->checkEventCompleted(chainReqId)) std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    std::cout << "Data retrieval started for underlying contract: " << contract.symbol << std::endl;
}

void UnderlyingData::stopReceivingData() {
    // Save daily data at the end
    wrapper->cancelMktData(mktDataId);
    wrapper->cancelRealTimeBars(rtbId);
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

std::string UnderlyingData::serializeKeyPricePoints() {
    Message message;
    message.set_type("underlying_contract");

    UnderlyingContract* underlyingContract = message.mutable_underlying_contract();
    underlyingContract->set_symbol(contract.symbol);

    UnderlyingAverages* underlyingAverages = underlyingContract->mutable_underlying_averages()->Add();
    underlyingAverages->set_low_13_week(low13Week);
    underlyingAverages->set_high_13_week(high13Week);
    underlyingAverages->set_low_26_week(low26week);
    underlyingAverages->set_high_26_week(high26Week);
    underlyingAverages->set_low_52_week(low52Week);
    underlyingAverages->set_high_52_week(high52Week);
    if (averageVolume90Day != -1) underlyingAverages->set_average_volume_90_day(averageVolume90Day);

    std::string serialized;
    message.SerializeToString(&serialized);

    return serialized;
}

std::string UnderlyingData::serializePriceTick(long time, double price) {
    Message message;
    message.set_type("underlying_contract");

    UnderlyingContract* underlyingContract = message.mutable_underlying_contract();
    underlyingContract->set_symbol(contract.symbol);

    UnderlyingPriceTick* underlyingPriceTick = underlyingContract->mutable_underlying_price_tick()->Add();
    underlyingPriceTick->set_time(time);
    underlyingPriceTick->set_price(price);

    std::string serialized;
    message.SerializeToString(&serialized);

    return serialized;
}

// Check if all or most of the key price points have reeceived tick data
bool UnderlyingData::verifyAveragesRecveived() {
    if (low13Week != -1 && high13Week != -1 && low26week != -1 && high26Week != -1 && low52Week != -1 && high52Week != -1) return true;
    else return false;
}

int UnderlyingData::requestOptionsChain() {
    std::string underlyingSymbol = contract.symbol;
    std::string futFopExchange = "";
    std::string underlyingSecType = contract.secType;
    int underlyingConId = ContractDefs::getConId(contract.symbol);
    std::cout << "Underlying Con ID: " << underlyingConId << std::endl;
    
    int optId = 0;
    wrapper->reqSecDefOptParams(
        [this, &optId](int reqId) {
            optId = reqId;
        },
        [this, &optId](const std::string& exchange, int underlyingConId, const std::string& tradingClass, 
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
double UnderlyingData::getLastPrice() { 
    std::lock_guard<std::mutex> lock(mtx);
    return currentPrice; 
}

std::pair<std::vector<double>, std::vector<double>> UnderlyingData::getStrikes(int countITM) {
    // Insert set items into vector for easier manipulation
    for (const auto& i : optionStrikes) optionsChain.push_back(i);
    // Get midpoint and find strike increment
    int mid = optionsChain.size() / 2;
    strikeIncrement = optionsChain[mid+1] - optionsChain[mid];
    
    // Loop until the first strike out of the money (for calls)
    int firstOTM = 0;
    for (int i=0; i < optionsChain.size(); i++) {
        if (currentPrice < optionsChain[i]) {
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
        // Send price to web socket
        currentPrice = event->price;
        sdc->sendUnderlyingContractData(serializePriceTick(event->timeStamp, event->price));
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
            //std::cout << "Tick Price type not used" << std::endl;
            break;
        }
    }

    if (outputData) event->print();
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
        //std::cout << "Tick Size type not used" << std::endl;
        break;
    }

    if (outputData) event->print();
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
        //std::cout << "Tick Generic type not used" << std::endl;
        break;
    }

    if (outputData) event->print();
}

// void UnderlyingData::handleTickNewsEvent(std::shared_ptr<TickNewsEvent> event) {
//     long timeStamp = event->dateTime;
//     // Check news tick map for time
//     auto it = newsTicks.find(timeStamp);
//     if (it == newsTicks.end()) {
//         std::pair<std::shared_ptr<TickNewsEvent>, double> lastTick{event, 0};
//         newsTicks.insert({timeStamp, lastTick});
//     } else {
//         it->second.first = event;
//     }

//     // Since there might be multiple news articles at the same time, we will just ignore the map
//     // and create objects for each event to save in the data table
//     ContractNewsData cnd;
//     cnd.time = timeStamp;
//     cnd.articleId = event->articleId;
//     cnd.headline = event->headline;
//     cnd.sentimentScore = event->sentimentScore;
//     cnd.price = currentPrice;
//     if (event->hasFullExtraData) sdc->sendNewsData(cnd.serializeNewsObject());
//     //if (event->hasFullExtraData) csv->addDataToQueue(contract.symbol, 0, "None", DataType::News, cnd.formatCSV());

//     event->print();
// }

void UnderlyingData::handleRealTimeCandles(std::shared_ptr<CandleDataEvent> event) {
    fiveSecData.push_back(event->candle);

    // If there are 12 candles, add to one min candle and clear vector
    if (fiveSecData.size() >= 12) {
        oneMinuteData.back().addFiveSecData(fiveSecData);

        fiveSecData.clear();

        // Serialize and send one minute candle to SocketDataCollector
        sdc->sendUnderlyingContractData(oneMinuteData.back().serializeOneMinData(contract.symbol));
        // If key price points have all been received, serialize and send that data as well
        if (verifyAveragesRecveived()) sdc->sendUnderlyingContractData(serializeKeyPricePoints());
        
        // Now add an empty one min candle to back of vector
        UnderlyingOneMinuteData oneMinCandle;
        oneMinuteData.push_back(oneMinCandle);
    }

    if (outputData) event->candle->printCandle();
}