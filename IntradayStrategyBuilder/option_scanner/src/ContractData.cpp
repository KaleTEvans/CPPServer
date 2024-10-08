#include "ContractData.h"

ContractData::ContractData(std::shared_ptr<tWrapper> wrapper, std::shared_ptr<CSVFileSaver> csv,
    std::shared_ptr<ScannerNotificationBus> notifications,
    int mktDataId, int rtbId, Contract contract, double strikeIncrement) : 
    mktDataId(mktDataId), csv(csv), notifications(notifications), rtbId(rtbId), contract(contract), 
    strikeIncrement(strikeIncrement), wrapper(wrapper) {
    // Subscribe to events
    wrapper->getMessageBus()->subscribe(EventType::TickPriceInfo, [this](std::shared_ptr<DataEvent> event) {
        // Use dynamic_pointer_cast to cast Event type to child class type
        if (event->getReqId() == this->mktDataId) this->handleTickPriceEvent(std::dynamic_pointer_cast<TickPriceEvent>(event));
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

    wrapper->getMessageBus()->subscribe(EventType::RealTimeCandleData, [this](std::shared_ptr<DataEvent> event) {
        if (event->getReqId() == this->rtbId) this->realTimeCandles(std::dynamic_pointer_cast<CandleDataEvent>(event));
    });

    csv->createDirectoriesAndFiles(contract.symbol, contract.strike, contract.right);

    std::cout << "ContractData Request IDs: " << mktDataId << "," << rtbId << " created for  contract: " <<
            contract.symbol << " " << contract.strike << contract.right << std::endl;
}

ContractData::~ContractData() {
    //cancelDataStream();
}

void ContractData::cancelDataStream() {
    wrapper->cancelMktData(mktDataId);
    wrapper->cancelRealTimeBars(rtbId);
    std::cout << "Market data and RTB streams have ended for Contract: " << contract.strike << contract.right << std::endl;
}

void ContractData::handleTickPriceEvent(std::shared_ptr<TickPriceEvent> event) {
    auto it = ticks.find(event->timeStamp);
    if (it == ticks.end()) {
        // Key does not exist, create a new MarketDataSingleFrame and insert it
        std::shared_ptr<MarketDataSingleFrame> mktData = std::make_shared<MarketDataSingleFrame>(event->timeStamp);
        mktData->inputTickPrice(event);
        ticks.insert({event->timeStamp, mktData});
    } else {
        // Key exists, update the existing MarketDataSingleFrame
        it->second->inputTickPrice(event);
    }
    if (outputData) event->print();

    // Update current bid and ask
    if (event->tickType == TickType::ASK) currentAsk = event->price;
    if (event->tickType == TickType::BID) currentBid = event->price;
}

void ContractData::handleTickSizeEvent(std::shared_ptr<TickSizeEvent> event) {
    auto it = ticks.find(event->timeStamp);
    if (it == ticks.end()) {
        // Key does not exist, create a new MarketDataSingleFrame and insert it
        std::shared_ptr<MarketDataSingleFrame> mktData = std::make_shared<MarketDataSingleFrame>(event->timeStamp);
        mktData->inputTickSize(event);
        ticks.insert({event->timeStamp, mktData});
    } else {
        // Key exists, update the existing MarketDataSingleFrame
        it->second->inputTickSize(event);
    }
    if (outputData) event->print();
}

void ContractData::handleTickStringEvent(std::shared_ptr<TickStringEvent> event) {
    // First, parse the string element if it is a RT value
    if (event->tickType == TickType::RT_VOLUME) {
        std::shared_ptr<TimeAndSales> tas = std::make_shared<TimeAndSales>(event->value);
        tradeSize_timeAndSales.addValue(tas->quantity);
        long timestamp = tas->timeValue;

        // Now, if trade size is over $10k, create a notification
        double priceInDollars = tas->price * 100;
        double totalTradePrice = priceInDollars * tas->quantity;
        if (totalTradePrice >= 10000) {
            auto largeOrder = std::make_shared<LargeOrderEvent>(contract, tas->timeValue, tas->price,
                tas->quantity, totalTradePrice, tas->totalVol, tas->vwap, currentAsk, currentBid, currentRtm);
            notifications->publish(largeOrder);
            csv->addDataToQueue(contract.symbol, contract.strike, 
                contract.right, DataType::LargeOrderAlert, largeOrder->formatCSV());
            
            if (tas->price >= currentAsk) largeOrder->printLargeOrder();
        }

        auto it = ticks.find(timestamp);
        if (it == ticks.end()) {
            // Key does not exist, create a new MarketDataSingleFrame and insert it
            std::shared_ptr<MarketDataSingleFrame> mktData = std::make_shared<MarketDataSingleFrame>(timestamp);
            mktData->inputTimeAndSales(tas);
            ticks.insert({timestamp, mktData});
        } else {
            // Key exists, update the existing MarketDataSingleFrame
            it->second->inputTimeAndSales(tas);
        }
    }
    if (outputData) event->print();
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
    if (event->tickType == TickType::LAST_OPTION_COMPUTATION) {
        lastUnderlyingPrice = event->undPrice;

        auto it = ticks.find(event->timeStamp);
        if (it == ticks.end()) {
            // Key does not exist, create a new MarketDataSingleFrame and insert it
            std::shared_ptr<MarketDataSingleFrame> mktData = std::make_shared<MarketDataSingleFrame>(event->timeStamp);
            mktData->inputTickOption(event);
            ticks.insert({event->timeStamp, mktData});
        } else {
            // Key exists, update the existing MarketDataSingleFrame
            it->second->inputTickOption(event);
        }

        // Update IV 
        currentIV = event->impliedVol;
    }
    if (outputData) event->print();
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
    currentRtm = rtm;

    std::shared_ptr<FiveSecondData> fsd = std::make_shared<FiveSecondData>(event->candle, rtm);
    fiveSecData[event->candle->time()] = fsd;

    // Check if candle falls on an even minute
    if (event->candle->time() % 60 == 0) isEvenMinute = true;

    // Save the five sec candle to csv
    csv->addDataToQueue(contract.symbol, contract.strike, contract.right, DataType::FiveSec, fsd->formatCSV());

    tradeCount_fiveSecCandles.addValue(event->candle->count());
    priceDelta_fiveSecCandles.addValue(event->candle->high() - event->candle->low());

    // Each time a five sec candle is created, run through ticks to drop into candles if any
    // If any ticks precede first candle, disregard
    int64_t firstCandleTime = fiveSecData.begin()->first; 

    // Creat a temporary vector to save the times of all the ticks added to a candle
    std::vector<int64_t> addedTickTimes;

    // Save most recent option data for one minute candles
    std::shared_ptr<MarketDataSingleFrame> recentOptData = nullptr;
    std::shared_ptr<MarketDataSingleFrame> recentTasData = nullptr;

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
                if (i.second->impliedVol != -100) recentOptData = i.second;
                // Do the same with aggregated volume
                if (i.second->tasPrice != -1) recentTasData = i.second;
            }
        } 

        // Save ticks to file
        csv->addDataToQueue(contract.symbol, contract.strike, contract.right, DataType::Tick, i.second->formatCSV(rtm));
        // Save time of tick for removal
        addedTickTimes.push_back(i.first);
    }
    
    // Next, we need to loop back through the ticks and remove them from the tick map to save space
    for (auto const& i : addedTickTimes) ticks.erase(i);

    // If an even minute has been accounted for, start adding candles to temp vector for one minute creation
    if (isEvenMinute) tempCandles.push_back(fsd);

    // If temp candle count is 12, create one minute candle
    if (tempCandles.size() == 12) {
        // Send to constructor
        std::shared_ptr<OneMinuteData> c = std::make_shared<OneMinuteData>(tempCandles, recentOptData, recentTasData);
        oneMinuteCandles.push_back(c);

        priceDelta_oneMinCandles.addValue(c->candle->high() - c->candle->low());

        // Save one minute candle to db
        csv->addDataToQueue(contract.symbol, contract.strike, contract.right, DataType::OneMin, c->formatCSV());

        // Clear temp candles and reset even minute flag
        tempCandles.clear();
        isEvenMinute = false;
    }
}

void ContractData::printData() { outputData = true; }

FiveSecondData::FiveSecondData(std::shared_ptr<Candle> candle, RelativeToMoney rtm) :
    candle(candle), rtm(rtm) {}

std::string FiveSecondData::formatCSV() {
    return std::to_string(candle->time()) + "," +
            std::to_string(candle->open()) + "," +
            std::to_string(candle->close()) + "," +
            std::to_string(candle->high()) + "," +
            std::to_string(candle->low()) + "," +
            decimalToString(candle->volume()) + "," +
            std::to_string(candle->count()) + "," +
            getRTMstr(rtm) + "\n";
}

OneMinuteData::OneMinuteData(std::vector<std::shared_ptr<FiveSecondData>> candles, 
    std::shared_ptr<MarketDataSingleFrame> optionInfo, std::shared_ptr<MarketDataSingleFrame> tasInfo) 
    {
    int n = candles.size()-1;
    auto& currentCandle = *candles[n];
    rtm = currentCandle.rtm;

    auto reqId = candles[0]->candle->reqId();
    auto time = candles[0]->candle->time();

    if (optionInfo != nullptr) {
        impliedVol = optionInfo->impliedVol;
        delta = optionInfo->delta;
        gamma = optionInfo->gamma;
        vega = optionInfo->vega;
        theta = optionInfo->theta;
        undPrice = optionInfo->undPrice;
    }
    if (tasInfo != nullptr) totalVol = tasInfo->tasTotalVol;

    // Now loop over 5 sec candles in vector to get olhc and vol
    double vol = 0;
    double high = 0;
    double low = 10000;
    double count = 0; // Count of tradees per candles
    double open = candles[0]->candle->open();
    double close = currentCandle.candle->close();

    for (auto i = 0; i < candles.size()-1; i++) {
        if (candles[i]->candle->high() >= high) high = candles[i]->candle->high();
        if (candles[i]->candle->low() <= low) low = candles[i]->candle->low();
        vol += decimalToDouble(candles[i]->candle->volume());
        count += candles[i]->candle->count();
    }

    candle = std::make_shared<Candle>(reqId, time, open, high, low, close, 0);
    candleVol = vol;
    tradeCount = count;
}

std::string OneMinuteData::formatCSV() {
    return std::to_string(candle->time()) + "," +
            std::to_string(candle->open()) + "," +
            std::to_string(candle->close()) + "," +
            std::to_string(candle->high()) + "," +
            std::to_string(candle->low()) + "," +
            std::to_string(candleVol) + "," +
            std::to_string(tradeCount) + "," +
            std::to_string(impliedVol) + "," +
            std::to_string(delta) + "," +
            std::to_string(gamma) + "," +
            std::to_string(vega) + "," +
            std::to_string(theta) + "," +
            std::to_string(undPrice) + "," +
            std::to_string(totalVol) + "," +
            getRTMstr(rtm) + "\n";
}

MarketDataSingleFrame::MarketDataSingleFrame(int64_t timestamp) : timestamp(timestamp) {}

void MarketDataSingleFrame::inputTickPrice(std::shared_ptr<TickPriceEvent> tickPrice) {
    TickType tickType = tickPrice->tickType;
    switch (tickType)
    {
    case TickType::ASK: 
        askPrice = tickPrice->price;
        break;
    case TickType::BID:
        bidPrice = tickPrice->price;
        break;
    case TickType::LAST:
        lastPrice = tickPrice->price;
        break;
    case TickType::MARK_PRICE:
        markPrice = tickPrice->price;
        break;
    
    default:
        //tickPrice->print();
        break;
    }
}

void MarketDataSingleFrame::inputTickSize(std::shared_ptr<TickSizeEvent> tickSize) {
    TickType tickType = tickSize->tickType;
    switch (tickType)
    {
    case TickType::ASK_SIZE:
        askSize = decimalToDouble(tickSize->size);
        break;
    case TickType::BID_SIZE:
        bidSize = decimalToDouble(tickSize->size);
        break;
    case TickType::VOLUME:
        volume = decimalToDouble(tickSize->size);
        break;
    
    default:
        //tickSize->print();
        break;
    }
}

void MarketDataSingleFrame::inputTickOption(std::shared_ptr<TickOptionComputationEvent> tickOption) {
    impliedVol = tickOption->impliedVol;
    delta = tickOption->delta;
    gamma = tickOption->gamma;
    vega = tickOption->vega;
    theta = tickOption->theta;
    undPrice = tickOption->undPrice;
    return;
}

void MarketDataSingleFrame::inputTimeAndSales(std::shared_ptr<TimeAndSales> timeAndSales) {
    tasPrice = timeAndSales->price;
    tasQuantity = timeAndSales->quantity;
    tasTotalVol = timeAndSales->totalVol;
    tasVWAP = timeAndSales->vwap;
    tasFilledBySingleMM = timeAndSales->filledBySingleMM;
    return;
}

std::string MarketDataSingleFrame::valueToCSV(double value) {
    return (value == -1 || value == -100) ? "" : std::to_string(value);
}

std::string MarketDataSingleFrame::formatCSV(RelativeToMoney rtm) {
    std::string filledByMM = "";
    if (tasFilledBySingleMM) filledByMM = "YES";
    else tasFilledBySingleMM = "NO";
    return std::to_string(timestamp) + "," +
               valueToCSV(bidPrice) + "," +
               valueToCSV(bidSize) + "," +
               valueToCSV(askPrice) + "," +
               valueToCSV(askSize) + "," +
               valueToCSV(lastPrice) + "," +
               valueToCSV(markPrice) + "," +
               valueToCSV(volume) + "," +
               valueToCSV(impliedVol) + "," +
               valueToCSV(delta) + "," +
               valueToCSV(gamma) + "," +
               valueToCSV(vega) + "," +
               valueToCSV(theta) + "," +
               valueToCSV(undPrice) + "," +
               valueToCSV(tasPrice) + "," +
               valueToCSV(tasQuantity) + "," +
               valueToCSV(tasTotalVol) + "," +
               valueToCSV(tasVWAP) + "," +
               getRTMstr(rtm) + "," +
               filledByMM + "\n";
}