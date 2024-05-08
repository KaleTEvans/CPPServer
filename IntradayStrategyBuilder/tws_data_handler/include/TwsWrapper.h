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


#define IB_USE_STD_STRING
/**/
#define IBString _IBString
#include "IBString.h"
#undef IBString

struct IBString: public _IBString
{
	IBString( void					) { reserve(32); }
	IBString( const char*		 s	) { this->assign(s); }
	IBString( const std::string& s	) { this->assign(s.data()); }

	operator const char*  (void) const{ return data(); }
};
/**/

#include "TwsEventHandler.h"
#include "EReaderOSSignal.h"
#include "EReader.h"
#include "ContractDefs.h"

class EClientSocket;

/////////////////////////////////////////////////////////////
// Enum class and struct for keeping track of open requests
/////////////////////////////////////////////////////////////

enum class RequestType {
    MktData,
    RealTimeBars,
    OptionPrice,
    OptionVol,
    Inactive
};

struct OpenRequest {
    OpenRequest(Contract con, RequestType rqt);
    OpenRequest();

    std::string getRequestType();

    Contract con{ContractDefs::emptyContract()};
    RequestType rqt{RequestType::Inactive};
};

class tWrapper : public EWrapper {

public:
    tWrapper();
    ~tWrapper();

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
    long getSystemTime(); // Get TWS system time

    //===========================================
    // EClient Subscriptions for Streaming Data
    //===========================================

    // Attempting to maintain simplicity here so that all requests and callbacks are contained within TWS Wrapper
    // Add new request functions as needed
    void reqCurrentTime();
    int reqMktData(const Contract& con, const std::string& genericTicks, bool snapshot, bool regulatorySnapshot);
    void cancelMktData(int reqId);
    int reqRealTimeBars(const Contract& con, int barSize, const std::string& whatToShow, bool useRTH);
    void cancelRealTimeBars(int reqId);

    // For keeping track of current requests
    Contract getContractById(int reqId); // Get the contract associated with a tick subscription
    void showOpenRequests();

    // Note for price and volatility calculations: Output will be in tickOptionComputation, only filter for 
    // price and volatility for these, everything else will come out as garbage
    void calculateOptionPrice(const Contract& con, double volatility, double underlyingPrice);
    void calculateImpliedVolatility(const Contract& con, double optionPrice, double underlyingPrice);

public:
    //==========================================
    // Callback Definitions
    //==========================================

    // Callback rerouting for all wrapper methods
    using CallbackID = std::function<void(int)>;
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
    std::map<int, OpenRequest> tickSubscribers; // This will map all generated IDs to associated contracts to help manage tick data

    std::map<int, EventContractDetails> contractDetailsSubscribers;
    std::map<int, EventSecDefOptParamns> optParamSubscribers;
    std::map<int, EventHistoricalData> historicalDataSubscribers;

public:
    //==========================================
    // Callback Subscriptions for Single Events
    //==========================================

    // All program components can subscribe to these methods to receive the specific data they need
    // Each has the option to generate a new ID for requests and subscription handling, or use a custom ID
    void reqContractDetails(CallbackID cbId, EventContractDetails event, const Contract& con);
    void reqSecDefOptParams(CallbackID cbId, EventSecDefOptParamns event, const std::string& underlyingSymbol, 
        const std::string& futFopExchange, const std::string& underlyingSecType, int underlyingConId);
    void reqHistoricalData(CallbackID cbId, EventHistoricalData event, const Contract& con, const std::string& endDate, 
        const std::string& durationStr, const std::string& barSizeSetting, const std::string& whatToShow, 
        int useRTH, int formatDate, bool keepUpToDate);

    bool checkEventCompleted(int reqId); // Check current requests map

private:
    // There should only be a single instance of the message bus associated with the wrapper
    std::shared_ptr<MessageBus> messageBus;
    // TWS system time will be automatically requested every second and stored in this variable
    long systemTime = 0;


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