#ifndef TWSWRAPPER_H
#define TWSWRAPPER_H

#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <condition_variable>
#include <set>

#include "TwsEventHandler.h"
using namespace TwsApi; // for TwsApiDefs.h

class tWrapper : public EWrapperL0 {

public:
    ///Easier: The EReader calls all methods automatically(optional)
    tWrapper(bool runEReader = true);

    // Public variables to determine completion of certain wrapper requests
    bool m_Done, m_ErrorForRequest;
    bool notDone(void) { return !(m_Done || m_ErrorForRequest); }

    //========================================
    // Message Bus
    //========================================
    std::shared_ptr<MessageBus> getMessageBus();

    //========================================
    // Error Handling
    //========================================

    ///Methods winError & error print the errors reported by IB TWS
    virtual void winError(const IBString& str, int lastError);
    virtual void error(const int id, const int errorCode, const IBString errorString);

    ///Safer: uncatched exceptions are catched before they reach the IB library code.
    ///       The Id is tickerId, orderId, or reqId, or -1 when no id known
    virtual void OnCatch(const char* MethodName, const long Id);

    //=======================================
    // Connectivity
    //=======================================

    virtual void connectionOpened();
    virtual void connectionClosed();

    // Upon initial API connection, recieves a comma-separated string with the managed account IDs
    virtual void managedAccounts(const IBString& accountsList);

    //========================================
    // Data Retrieval
    //========================================

    virtual void currentTime(long time);

    virtual void contractDetails(int reqId, const ContractDetails& contractDetails);
    virtual void contractDetailsEnd(int reqId);

    /////// Tick Options /////////
    virtual void tickPrice(TickerId tickerId, TickType field, double price, int canAutoExecute);
    virtual void tickGeneric(TickerId tickerId, TickType tickType, double value);
    virtual void tickSize(TickerId tickerId, TickType field, int size);
    virtual void marketDataType(TickerId reqId, int marketDataType);
    virtual void tickString(TickerId tickerId, TickType tickType, const IBString& value);
    virtual void tickSnapshotEnd(int reqId);

    virtual void historicalData(TickerId reqId, const IBString& date
        , double open, double high, double low, double close
        , int volume, int barCount, double WAP, int hasGaps);

    // Retrieve real time bars, TWS Api currently only returns 5 second candles
    virtual void realtimeBar(TickerId reqId, long time, double open, double high,
        double low, double close, long volume, double wap, int count);

    virtual void cancelRealTimeBars(TickerId tickerId) {
        std::cout << "Cancelled real time bar data for " << tickerId << std::endl;
    }

    long getCurrentime(long time);
    std::pair<TickerId, double> getLastPrice(TickerId reqId, double price);

    std::mutex& wrapperMutex();
    std::condition_variable& wrapperConditional();

private:
    // There should only be a single instance of the message bus associated with the wrapper
    std::shared_ptr<MessageBus> messageBus;

    std::mutex wrapperMtx_;
    std::condition_variable cv_;

    // These simple types will only be updated periodically, and rather than sending to the bus
    // will just provide direct access from the wrapper
    long time{0};
    std::pair<TickerId, double> lastPrice{0, 0};
};


#endif