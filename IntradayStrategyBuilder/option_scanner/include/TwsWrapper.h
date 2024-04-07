#ifndef TWSWRAPPER_H
#define TWSWRAPPER_H

////////////////////////////////////////////////////////////////////////////////////////////////////
// TWS Wrapper
// Receives data from the tws EReader and translates to readable data via the wrapper functions.
// via the wrapper functions. As the data is received, it is then packaged into a new type and 
// sent to the message bus (found in TwsEventHandler.h) for centralized event data distribution
// across the program
//
// NOTE: If adding new wrapper functions, must create new DataEvent classes to be sent to  
// the message bus if data is of different type than any existing classes.
////////////////////////////////////////////////////////////////////////////////////////////////////

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
#include "EReaderOSSignal.h"
#include "EReader.h"

class EClientSocket;

class tWrapper : public EWrapper {

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
    virtual void winError(const std::string& str, int lastError);
    virtual void error(const int id, const int errorCode, const std::string& errorString, const std::string& advancedOrderRejectJson);

    ///Safer: uncatched exceptions are catched before they reach the IB library code.
    ///       The Id is tickerId, orderId, or reqId, or -1 when no id known
    virtual void OnCatch(const char* MethodName, const long Id);

    //=======================================
    // Connectivity
    //=======================================

    virtual void connectionClosed();

    // Upon initial API connection, recieves a comma-separated string with the managed account IDs
    virtual void managedAccounts(const std::string& accountsList);

    //========================================
    // Data Retrieval
    //========================================

    virtual void currentTime(long time);

    virtual void contractDetails(int reqId, const ContractDetails& contractDetails);
    virtual void contractDetailsEnd(int reqId);

    /////// Tick Options /////////
    virtual void tickPrice(TickerId tickerId, TickType field, double price, const TickAttrib& attrib);
    virtual void tickGeneric(TickerId tickerId, TickType tickType, double value);
    virtual void tickSize(TickerId tickerId, TickType field, int size);
    virtual void marketDataType(TickerId reqId, int marketDataType);
    virtual void tickString(TickerId tickerId, TickType tickType, const std::string& value);
    virtual void tickSnapshotEnd(int reqId);
    virtual void tickNews(int tickerId, time_t timeStamp, const std::string& providerCode, 
        const std::string& articleId, const std::string& headline, const std::string& extraData);

    virtual void historicalData(TickerId reqId, const Bar& bar);

    // Retrieve real time bars, TWS Api currently only returns 5 second candles
    virtual void realtimeBar(TickerId reqId, long time, double open, double high,
        double low, double close, long volume, double wap, int count);

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


public:
    //#include "EWrapper_prototypes.h"
};


#endif