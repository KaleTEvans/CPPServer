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
#include <unordered_map>
#include <condition_variable>
#include <set>
#include <thread>

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

    long getCurrentime(long time);

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
    // EClient Request Functions
    //===========================================

    // Attempting to maintain simplicity here so that all requests and callbacks are contained within TWS Wrapper
    // Add new request functions as needed

    void reqMktData(int reqId, const Contract& con, const std::string& genericTicks, bool snapshot, bool regulatorySnapshot);
    void cancelMktData(int reqId);
    void reqContractDetails(int reqId, const Contract& con);
    void reqSecDefOptParams(int reqId, const std::string& underlyingSymbol, const std::string& futFopExchange, const std::string& underlyingSecType, int underlyingConId);


private:
    // There should only be a single instance of the message bus associated with the wrapper
    std::shared_ptr<MessageBus> messageBus;

    // These simple types will only be updated periodically, and rather than sending to the bus
    // will just provide direct access from the wrapper
    long time_{0};


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