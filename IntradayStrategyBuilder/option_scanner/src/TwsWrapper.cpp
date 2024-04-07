#include "TwsWrapper.h"

tWrapper::tWrapper(bool runEReader) : messageBus(std::make_shared<MessageBus>())
{
    m_Done = false;
    m_ErrorForRequest = false;
}

std::shared_ptr<MessageBus> tWrapper::getMessageBus() { return messageBus; }

//==================== Error Handling ========================

///Methods winError & error print the errors reported by IB TWS
void tWrapper::winError(const std::string& str, int lastError) {
    fprintf(stderr, "WinError: %d = %s\n", lastError, str.c_str());
    m_ErrorForRequest = true;
}

void tWrapper::error(const int id, const int errorCode, const std::string& errorString, const std::string& advancedOrderRejectJson) {
    if (!advancedOrderRejectJson.empty()) {
        printf("Error. Id: %d, Code: %d, Msg: %s, AdvancedOrderRejectJson: %s\n", id, errorCode, errorString.c_str(), advancedOrderRejectJson.c_str());
    } else {
        printf("Error. Id: %d, Code: %d, Msg: %s\n", id, errorCode, errorString.c_str());
    }
}

///Safer: uncatched exceptions are catched before they reach the IB library code.
///       The Id is tickerId, orderId, or reqId, or -1 when no id known
void tWrapper::OnCatch(const char* MethodName, const long Id) {
    fprintf(stderr, "*** Catch in EWrapper::%s( Id=%ld, ...) \n", MethodName, Id);
}

//======================== Connectivity =============================

void tWrapper::connectionClosed() { std::cout << "Connection has been closed" << std::endl; }

// Upon initial API connection, recieves a comma-separated string with the managed account IDs
void tWrapper::managedAccounts(const std::string& accountsList) { std::cout << accountsList << std::endl; }

// ================== tWrapper callback functions =======================

void tWrapper::currentTime(long time) { 
    std::cout << "Current Time: " << time << std::endl;
    time = time;
}

void tWrapper::contractDetails(int reqId, const ContractDetails& contractDetails) {
    auto event = std::make_shared<ContractDataEvent>(reqId, contractDetails);
    messageBus->publish(event);
}

void tWrapper::contractDetailsEnd(int reqId) {
    auto event = std::make_shared<EndOfRequestEvent>(reqId);
    messageBus->publish(event);
}

void tWrapper::tickPrice(TickerId tickerId, TickType field, double price, const TickAttrib& attrib) {
    auto event = std::make_shared<TickPriceEvent>(tickerId, field, price, attrib);
    messageBus->publish(event);
}

void tWrapper::tickGeneric(TickerId tickerId, TickType tickType, double value) {
    auto event = std::make_shared<TickGenericEvent>(tickerId, tickType, value);
    messageBus->publish(event);
}

void tWrapper::tickSize(TickerId tickerId, TickType field, int size) {
    auto event = std::make_shared<TickSizeEvent>(tickerId, field, size);
    messageBus->publish(event);
}

void tWrapper::marketDataType(TickerId reqId, int marketDataType) {
    //std::cout << "Market data type: " << marketDataType << std::endl;
}

void tWrapper::tickString(TickerId tickerId, TickType tickType, const std::string& value) {
    
}

void tWrapper::tickSnapshotEnd(int reqId) {
    auto event = std::make_shared<EndOfRequestEvent>(reqId);
    messageBus->publish(event);
}

void tWrapper::tickNews(int tickerId, time_t timeStamp, const std::string& providerCode, 
    const std::string& articleId, const std::string& headline, const std::string& extraData) {
        
    }

void tWrapper::historicalData(TickerId reqId, const Bar& bar) {

    // Need to cast volume as a long
    long vol = static_cast<long>(bar.volume);
    std::shared_ptr<Candle> candle = std::make_shared<Candle>(
        reqId, bar.time, bar.open, bar.high, bar.low, bar.close, vol, bar.count, bar.wap
    );

    auto event = std::make_shared<HistoricalCandleDataEvent>(reqId, candle);
    messageBus->publish(event);
}

void tWrapper::realtimeBar(TickerId reqId, long time, double open, double high,
    double low, double close, long volume, double wap, int count) {

    std::shared_ptr<Candle> candle = std::make_shared<Candle>(
        reqId, time, open, high, low, close, volume, wap, count
    );

    auto event = std::make_shared<HistoricalCandleDataEvent>(reqId, candle);
    messageBus->publish(event);
}

long tWrapper::getCurrentime(long time) { return time; }
std::pair<TickerId, double> tWrapper::getLastPrice(TickerId reqId, double price) { return {reqId, price}; }