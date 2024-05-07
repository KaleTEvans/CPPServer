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
    if (!fiveSecCandles.empty()) {
        auto& currentCandle = *fiveSecCandles.back();
        auto it = currentCandle.ticks.find(event->timeStamp);
        if (it == currentCandle.ticks.end()) {
            // Key does not exist, create a new MarketDataSingleFrame and insert it
            MarketDataSingleFrame mktData(event->timeStamp);
            mktData.tickPrice = event;
            currentCandle.ticks.insert({event->timeStamp, mktData});
        } else {
            // Key exists, update the existing MarketDataSingleFrame
            it->second.tickPrice = event;
        }
    }
}

void ContractData::handleTickGenericEvent(std::shared_ptr<TickGenericEvent> event) {
    if (!fiveSecCandles.empty()) {
        auto& currentCandle = *fiveSecCandles.back();
        auto it = currentCandle.ticks.find(event->timeStamp);
        if (it == currentCandle.ticks.end()) {
            // Key does not exist, create a new MarketDataSingleFrame and insert it
            MarketDataSingleFrame mktData(event->timeStamp);
            mktData.tickGeneric = event;
            currentCandle.ticks.insert({event->timeStamp, mktData});
        } else {
            // Key exists, update the existing MarketDataSingleFrame
            it->second.tickGeneric = event;
        }
    }
}

void ContractData::handleTickSizeEvent(std::shared_ptr<TickSizeEvent> event) {
    if (!fiveSecCandles.empty()) {
        auto& currentCandle = *fiveSecCandles.back();
        auto it = currentCandle.ticks.find(event->timeStamp);
        if (it == currentCandle.ticks.end()) {
            // Key does not exist, create a new MarketDataSingleFrame and insert it
            MarketDataSingleFrame mktData(event->timeStamp);
            mktData.tickSize = event;
            currentCandle.ticks.insert({event->timeStamp, mktData});
        } else {
            // Key exists, update the existing MarketDataSingleFrame
            it->second.tickSize = event;
        }
    }
}

void ContractData::handleTickStringEvent(std::shared_ptr<TickStringEvent> event) {
    if (!fiveSecCandles.empty()) {
        auto& currentCandle = *fiveSecCandles.back();
        auto it = currentCandle.ticks.find(event->timeStamp);
        if (it == currentCandle.ticks.end()) {
            // Key does not exist, create a new MarketDataSingleFrame and insert it
            MarketDataSingleFrame mktData(event->timeStamp);
            mktData.tickString = event;
            currentCandle.ticks.insert({event->timeStamp, mktData});
        } else {
            // Key exists, update the existing MarketDataSingleFrame
            it->second.tickString = event;
        }
    }
}

void ContractData::tickOptionInfo(std::shared_ptr<TickOptionComputationEvent> event) {
    lastUnderlyingPrice = event->undPrice;
    std::cout << event->undPrice << std::endl;
    if (!fiveSecCandles.empty()) {
        auto& currentCandle = *fiveSecCandles.back();
        auto it = currentCandle.ticks.find(event->timeStamp);
        if (it == currentCandle.ticks.end()) {
            // Key does not exist, create a new MarketDataSingleFrame and insert it
            MarketDataSingleFrame mktData(event->timeStamp);
            mktData.tickOption = event;
            currentCandle.ticks.insert({event->timeStamp, mktData});
        } else {
            // Key exists, update the existing MarketDataSingleFrame
            it->second.tickOption = event;
        }
    }
}

void ContractData::tickNewsEvent(std::shared_ptr<TickNewsEvent> event) {
    if (fiveSecCandles.size() > 0) {
        auto& currentCandle = *fiveSecCandles.back();
        currentCandle.tickNews.push_back(event);
    }
}

void ContractData::realTimeCandles(std::shared_ptr<CandleDataEvent> event) {
    double priceDiff = contract.strike - lastUnderlyingPrice;
    double multiple = priceDiff / strikeIncrement;
    double strike = 0;

    std::cout << "Strike: " << multiple << std::endl;
    std::cout << "CD Last Price: " << lastUnderlyingPrice << std::endl;

    if (multiple >= 0) strike = std::ceil(multiple);
    else strike = std::floor(multiple);

    // If a call, positive strike means ITM
    RelativeToMoney rtm = RelativeToMoney::NoValue;
    if (contract.right == "C") rtm = getRTM(strike * -1);
    else rtm = getRTM(strike);

    std::shared_ptr<FiveSecondData> fsd = std::make_shared<FiveSecondData>(event->candle, rtm);
    fiveSecCandles.push_back(fsd);

    std::cout << "New 5 Sec Candle Added" << std::endl;
}

void ContractData::printData() {
    std::cout << "==============================================================" << std::endl;
    for (auto& i : fiveSecCandles) {
        i->candle->printCandle();
        std::cout << "--------------------------------------------------------------" << std::endl;
        for (auto& j : i->ticks) {
            j.second.printMktData();
        }
    }
}

FiveSecondData::FiveSecondData(std::shared_ptr<Candle> candle, RelativeToMoney rtm) :
    candle(candle), rtm(rtm) {
        candle->printCandle();
        std::cout << "Relative To Money: " << tag_to_db_key(rtm) << std::endl;
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

    if (tickOption != nullptr) {
        printf("%lu | TickOptionComputation. Ticker Id: %d, Type: %s, TickAttrib: %d," 
        "ImpliedVolatility: %f, Delta: %f, OptionPrice: %f, pvDividend: %f, Gamma: %f," 
        "Vega: %f, Theta: %f, Underlying Price: %f\n", tickOption->timeStamp, tickOption->reqId, 
        *(TickTypes::ENUMS)tickOption->tickType, tickOption->tickAttrib, tickOption->impliedVol, tickOption->delta, tickOption->optPrice, tickOption->pvDividend, 
        tickOption->gamma, tickOption->vega, tickOption->theta, tickOption->undPrice);
    }

    std::cout << "-------------------------------------------------------------------------" << std::endl;
}