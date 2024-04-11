#pragma once
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
#include <map>
#include <condition_variable>
#include <set>
#include <thread>
#include <functional>
#include <mutex>
#include <atomic>
#include <queue>

#include "TwsEventHandler.h"
#include "EReaderOSSignal.h"
#include "EReader.h"

class EClientSocket;

class tWrapper : public EWrapper {

public:
    tWrapper();
    ~tWrapper();

    // Public variables to determine completion of certain wrapper requests
    bool m_Done, m_ErrorForRequest;
    bool notDone(void) { return !(m_Done || m_ErrorForRequest); }

    //========================================
    // Message Bus
    //========================================
    std::shared_ptr<MessageBus> getMessageBus();

    //===========================================
    // Connectivity
    //===========================================

    void setConnectOptions(const std::string&);

    bool connect(const char * host, int port, int clientId = 0);
	void disconnect();
	bool isConnected() const;

    //===========================================
    // Message Processing
    //===========================================

    void processMessages(); // One-time message processing
    void processMsgLoop(); // Continuous message processing
    void startMsgProcessingThread();

    //===========================================
    // EClient Subscriptions for Streaming Data
    //===========================================

    // Attempting to maintain simplicity here so that all requests and callbacks are contained within TWS Wrapper
    // Add new request functions as needed
    void reqMktData(int reqId, const Contract& con, const std::string& genericTicks, bool snapshot, bool regulatorySnapshot);
    void cancelMktData(int reqId);
    void reqRealTimeBars(int reqId, const Contract& con, int barSize, const std::string& whatToShow, bool useRTH);
    void cancelRealTimeBars(int reqId);

public:
    //==========================================
    // Callback Definitions
    //==========================================

    // Callback rerouting for all wrapper methods
    using EventCurrentTime = std::function<void(long)>;
    using EventContractDetails = std::function<void(const ContractDetails&)>;
    using EventSecDefOptParamns = std::function<void(const std::string&, int, const std::string&, 
        const std::string&, const std::set<std::string>&, const std::set<double>&)>;
    using EventHistoricalData = std::function<void(std::shared_ptr<Candle>)>;

private:
    //==========================================
    // Callback lists
    //==========================================

    std::mutex mtx;
    std::atomic<int> subscriptionId{0}; // Atomic counter for subscription IDs
    std::map<int, bool> currentRequests; // Incomplete requests will be false, completed true

    std::queue<EventCurrentTime> currentTimeSubscribers; // Events with no reqId will use a queue
    std::map<int, EventContractDetails> contractDetailsSubscribers;
    std::map<int, EventSecDefOptParamns> optParamSubscribers;
    std::map<int, EventHistoricalData> historicalDataSubscribers;

public:
    //==========================================
    // Callback Subscriptions for Single Events
    //==========================================

    // All program components can subscribe to these methods to receive the specific data they need
    // Each has the option to generate a new ID for requests and subscription handling, or use a custom ID
    void reqCurrentTime(EventCurrentTime event);
    void reqContractDetails(EventContractDetails event, const Contract& con);
    void reqSecDefOptParams(EventSecDefOptParamns event, const std::string& underlyingSymbol, const std::string& futFopExchange, 
        const std::string& underlyingSecType, int underlyingConId);
    void reqHistoricalData(EventHistoricalData event, const Contract& con, const std::string& endDate, 
        const std::string& durationStr, const std::string& barSizeSetting, const std::string& whatToShow, 
        int useRTH, int formatDate, bool keepUpToDate);

    bool checkEventCompleted(int reqId); // Check current requests map
    void unsubscribeFromEvent(int subId); // EndOfRequest will automatically call unsubscribe

private:
    // There should only be a single instance of the message bus associated with the wrapper
    std::shared_ptr<MessageBus> messageBus;


public:
    #include "EWrapper_prototypes.h"

private:
    //! [socket_declare]
    EReaderOSSignal m_osSignal;
	EClientSocket * const m_pClient;
	//! [socket_declare]
	time_t m_sleepDeadline;

	OrderId m_orderId;
	std::unique_ptr<EReader> m_pReader;
    bool m_extraAuth;

    // Thread for continuously checking messages
    std::thread msgProcessingThread;
};


#endif