#include "TwsWrapper.h"

tWrapper::tWrapper(bool runEReader) : EWrapperL0(runEReader), messageBus(std::make_shared<MessageBus>())
{
    m_Done = false;
    m_ErrorForRequest = false;
}

std::shared_ptr<MessageBus> tWrapper::getMessageBus() { return messageBus; }

//==================== Error Handling ========================

///Methods winError & error print the errors reported by IB TWS
void tWrapper::winError(const IBString& str, int lastError) {
    fprintf(stderr, "WinError: %d = %s\n", lastError, (const char*)str);
    m_ErrorForRequest = true;
}

void tWrapper::error(const int id, const int errorCode, const IBString errorString) {
    if (errorCode != 2176) { // 2176 is a weird api error that claims to not allow use of fractional shares
        fprintf(stderr, "Error for id=%d: %d = %s\n"
            , id, errorCode, (const char*)errorString);
        m_ErrorForRequest = (id > 0);    // id == -1 are 'system' messages, not for user requests
    }
}

///Safer: uncatched exceptions are catched before they reach the IB library code.
///       The Id is tickerId, orderId, or reqId, or -1 when no id known
void tWrapper::OnCatch(const char* MethodName, const long Id) {
    fprintf(stderr, "*** Catch in EWrapper::%s( Id=%ld, ...) \n", MethodName, Id);
}

//======================== Connectivity =============================

void tWrapper::connectionOpened() { std::cout << "Connected to TWS" << std::endl; }
void tWrapper::connectionClosed() { std::cout << "Connection has been closed" << std::endl; }

// Upon initial API connection, recieves a comma-separated string with the managed account IDs
void tWrapper::managedAccounts(const IBString& accountsList) { std::cout << accountsList << std::endl; }

// ================== tWrapper callback functions =======================

void tWrapper::currentTime(long time) { 
    std::cout << "Current Time: " << time << std::endl;
    time = time;
}

void tWrapper::contractDetails(int reqId, const ContractDetails& contractDetails) {
    std::cout << "Contract Details received" << std::endl;
    auto event = std::make_shared<ContractDataEvent>(reqId, contractDetails);
    messageBus->publish(event);
}

void tWrapper::contractDetailsEnd(int reqId) {
    auto event = std::make_shared<EndOfRequestEvent>(reqId);
    messageBus->publish(event);
}

void tWrapper::tickPrice(TickerId tickerId, TickType field, double price, int canAutoExecute) {
    if (field == TickType::LAST) lastPrice = {tickerId, price};
    // std::cout << "Field: " << field << " Price: " << price << std::endl;
}

void tWrapper::tickGeneric(TickerId tickerId, TickType tickType, double value) {
    // std::cout << "Tick Type: " << tickType << " Value: " << value << std::endl;
}

void tWrapper::tickSize(TickerId tickerId, TickType field, int size) {
    //std::cout << "Tick Size: " << size << std::endl;
}

void tWrapper::marketDataType(TickerId reqId, int marketDataType) {
    //std::cout << "Market data type: " << marketDataType << std::endl;
}

void tWrapper::tickString(TickerId tickerId, TickType tickType, const IBString& value) {
    // std::cout << "Value: " << value << std::endl;
}

void tWrapper::tickSnapshotEnd(int reqId) {
    std::cout << "Tick snapshot end for reqID: " << reqId << std::endl;
}

void tWrapper::historicalData(TickerId reqId, const IBString& date
    , double open, double high, double low, double close
    , int volume, int barCount, double WAP, int hasGaps) {

    // Need to cast volume as a long
    long vol = static_cast<long>(volume);
    std::shared_ptr<Candle> candle = std::make_shared<Candle>(
        reqId, date, open, high, low, close, vol, barCount, WAP, hasGaps
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