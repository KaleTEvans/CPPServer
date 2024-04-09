#include "TwsWrapper.h"

#include "EClientSocket.h"
#include "EPosixClientSocketPlatform.h"

const int PING_DEADLINE = 2; // seconds
const int SLEEP_BETWEEN_PINGS = 30; // seconds

tWrapper::tWrapper() : messageBus(std::make_shared<MessageBus>())
    , m_osSignal(2000)//2-seconds timeout
    , m_pClient(new EClientSocket(this, &m_osSignal))
	, m_sleepDeadline(0)
	, m_orderId(0)
    , m_extraAuth(false)
{
    m_Done = false;
    m_ErrorForRequest = false;
}

tWrapper::~tWrapper() {
    // destroy the reader before the client
	if( m_pReader )
		m_pReader.reset();

	delete m_pClient;
}

std::shared_ptr<MessageBus> tWrapper::getMessageBus() { return messageBus; }

//===============================================================
// Connectivity
//===============================================================

void tWrapper::setConnectOptions(const std::string& connectOptions) {
	m_pClient->setConnectOptions(connectOptions);
}

bool tWrapper::connect(const char *host, int port, int clientId) {
	// trying to connect
	printf( "Connecting to %s:%d clientId:%d\n", !( host && *host) ? "127.0.0.1" : host, port, clientId);
	
	//! [connect]
	bool bRes = m_pClient->eConnect( host, port, clientId, m_extraAuth);
	//! [connect]
	
	if (bRes) {
		printf( "Connected to %s:%d clientId:%d\n", m_pClient->host().c_str(), m_pClient->port(), clientId);
		//! [ereader]
		m_pReader = std::unique_ptr<EReader>( new EReader(m_pClient, &m_osSignal) );
		m_pReader->start();
		//! [ereader]
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	else
		printf( "Cannot connect to %s:%d clientId:%d\n", m_pClient->host().c_str(), m_pClient->port(), clientId);

	return bRes;
}

void tWrapper::disconnect() {
	m_pClient->eDisconnect();
    if (msgProcessingThread.joinable()) msgProcessingThread.join();

	printf ( "Disconnected\n");
}

bool tWrapper::isConnected() const {
	return m_pClient->isConnected();
}

//================================================================
// Message Processing
//================================================================

void tWrapper::processMessages() {
    time_t now = time(NULL);

    m_osSignal.waitForSignal();
	errno = 0;
	m_pReader->processMsgs();
}

void tWrapper::processMsgLoop() {
    while (m_pClient->isConnected()) {
        m_pReader->processMsgs();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

// Thread will stop once client is disconnected from tws
void tWrapper::startMsgProcessingThread() {
    msgProcessingThread = std::thread([this]() {
        processMsgLoop();
    });
}

//================================================================
// Error Handling 
//================================================================

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

//================================================================
// Connectivity 
//================================================================

void tWrapper::connectionClosed() { std::cout << "Connection has been closed" << std::endl; }

// Upon initial API connection, recieves a comma-separated string with the managed account IDs
void tWrapper::managedAccounts(const std::string& accountsList) { std::cout << accountsList << std::endl; }

//==================================================================
// EClient Request Functions
//==================================================================

void tWrapper::reqMktData(int reqId, const Contract& con, const std::string& genericTicks, bool snapshot, bool regulatorySnapshot) {
    m_pClient->reqMktData(reqId, con, genericTicks, snapshot, regulatorySnapshot, TagValueListSPtr());
}

void tWrapper::cancelMktData(int reqId) { m_pClient->cancelMktData(reqId); }
void tWrapper::reqContractDetails(int reqId, const Contract& con) { m_pClient->reqContractDetails(reqId, con); }
void tWrapper::reqSecDefOptParams(int reqId, const std::string& underlyingSymbol, 
    const std::string& futFopExchange, const std::string& underlyingSecType, int underlyingConId) {
        m_pClient->reqSecDefOptParams(reqId, underlyingSymbol, futFopExchange, underlyingSecType, underlyingConId);
    }

//================================================================
// Wrapper callback functions 
//================================================================

void tWrapper::currentTime(long time) { 
    std::cout << "Current Time: " << time << std::endl;
    time_ = time;
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

void tWrapper::tickSize(TickerId tickerId, TickType field, Decimal size) {
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
        auto event = std::make_shared<TickNewsEvent>(tickerId, timeStamp, providerCode, articleId,
        headline, extraData);
        messageBus->publish(event);
    }

void tWrapper::securityDefinitionOptionalParameter(int reqId, const std::string& exchange, 
    int underlyingConId, const std::string& tradingClass, const std::string& multiplier, 
    const std::set<std::string>& expirations, const std::set<double>& strikes) {
        std::cout << "Exchange: " << exchange << " Trading Class: " << tradingClass << " multiplier: " << 
            multiplier << std::endl;

        std::cout << "Strikes" << std::endl;
        for (auto& i : strikes) {
            std::cout << i << " ";
        }
        std::cout << std::endl;

        std::cout << "Expirations" << std::endl;
        for (auto& i : expirations) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }

void tWrapper::securityDefinitionOptionalParameterEnd(int reqId) {
    auto event = std::make_shared<EndOfRequestEvent>(reqId);
    messageBus->publish(event);
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

void tWrapper::realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
	Decimal volume, Decimal wap, int count) {

    std::shared_ptr<Candle> candle = std::make_shared<Candle>(
        reqId, time, open, high, low, close, volume, wap, count
    );

    auto event = std::make_shared<HistoricalCandleDataEvent>(reqId, candle);
    messageBus->publish(event);
}

long tWrapper::getCurrentime(long time) { return time; }

///////////////////////////////////////////////////////////////////////////
// All additional virtual function definitions. Move to top if adding 
// any new data requests as needed
////////////////////////////////////////////////////////////////////////////

void tWrapper::tickOptionComputation( TickerId tickerId, TickType tickType, int tickAttrib, double impliedVol, double delta,
double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice) {}
void tWrapper::tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const std::string& formattedBasisPoints,
double totalDividends, int holdDays, const std::string& futureLastTradeDate, double dividendImpact, double dividendsToLastTradeDate) {}
void tWrapper::orderStatus( OrderId orderId, const std::string& status, Decimal filled,
Decimal remaining, double avgFillPrice, int permId, int parentId,
double lastFillPrice, int clientId, const std::string& whyHeld, double mktCapPrice) {}
void tWrapper::openOrder( OrderId orderId, const Contract&, const Order&, const OrderState&) {}
void tWrapper::openOrderEnd() {}
void tWrapper::updateAccountValue(const std::string& key, const std::string& val,
const std::string& currency, const std::string& accountName) {}
void tWrapper::updatePortfolio( const Contract& contract, Decimal position,
double marketPrice, double marketValue, double averageCost,
double unrealizedPNL, double realizedPNL, const std::string& accountName) {}
void tWrapper::updateAccountTime(const std::string& timeStamp) {}
void tWrapper::accountDownloadEnd(const std::string& accountName) {}
void tWrapper::nextValidId( OrderId orderId) {}
void tWrapper::bondContractDetails( int reqId, const ContractDetails& contractDetails) {}
void tWrapper::execDetails( int reqId, const Contract& contract, const Execution& execution) {}
void tWrapper::execDetailsEnd( int reqId) {}
void tWrapper::updateMktDepth(TickerId id, int position, int operation, int side,
double price, Decimal size) {}
void tWrapper::updateMktDepthL2(TickerId id, int position, const std::string& marketMaker, int operation,
int side, double price, Decimal size, bool isSmartDepth) {}
void tWrapper::updateNewsBulletin(int msgId, int msgType, const std::string& newsMessage, const std::string& originExch) {}
void tWrapper::receiveFA(faDataType pFaDataType, const std::string& cxml) {}
void tWrapper::historicalDataEnd(int reqId, const std::string& startDateStr, const std::string& endDateStr) {}
void tWrapper::scannerParameters(const std::string& xml) {}
void tWrapper::scannerData(int reqId, int rank, const ContractDetails& contractDetails,
const std::string& distance, const std::string& benchmark, const std::string& projection,
const std::string& legsStr) {}
void tWrapper::scannerDataEnd(int reqId) {}
void tWrapper::fundamentalData(TickerId reqId, const std::string& data) {}
void tWrapper::deltaNeutralValidation(int reqId, const DeltaNeutralContract& deltaNeutralContract) {}
void tWrapper::commissionReport( const CommissionReport& commissionReport) {}
void tWrapper::position( const std::string& account, const Contract& contract, Decimal position, double avgCost) {}
void tWrapper::positionEnd() {}
void tWrapper::accountSummary( int reqId, const std::string& account, const std::string& tag, const std::string& value, const std::string& curency) {}
void tWrapper::accountSummaryEnd( int reqId) {}
void tWrapper::verifyMessageAPI( const std::string& apiData) {}
void tWrapper::verifyCompleted( bool isSuccessful, const std::string& errorText) {}
void tWrapper::displayGroupList( int reqId, const std::string& groups) {}
void tWrapper::displayGroupUpdated( int reqId, const std::string& contractInfo) {}
void tWrapper::verifyAndAuthMessageAPI( const std::string& apiData, const std::string& xyzChallange) {}
void tWrapper::verifyAndAuthCompleted( bool isSuccessful, const std::string& errorText) {}
void tWrapper::connectAck() {}
void tWrapper::positionMulti( int reqId, const std::string& account,const std::string& modelCode, const Contract& contract, Decimal pos, double avgCost) {}
void tWrapper::positionMultiEnd( int reqId) {}
void tWrapper::accountUpdateMulti( int reqId, const std::string& account, const std::string& modelCode, const std::string& key, const std::string& value, const std::string& currency) {}
void tWrapper::accountUpdateMultiEnd( int reqId) {}
void tWrapper::softDollarTiers(int reqId, const std::vector<SoftDollarTier> &tiers) {}
void tWrapper::familyCodes(const std::vector<FamilyCode> &familyCodes) {}
void tWrapper::symbolSamples(int reqId, const std::vector<ContractDescription> &contractDescriptions) {}
void tWrapper::mktDepthExchanges(const std::vector<DepthMktDataDescription> &depthMktDataDescriptions) {}
void tWrapper::smartComponents(int reqId, const SmartComponentsMap& theMap) {}
void tWrapper::tickReqParams(int tickerId, double minTick, const std::string& bboExchange, int snapshotPermissions) {}
void tWrapper::newsProviders(const std::vector<NewsProvider> &newsProviders) {}
void tWrapper::newsArticle(int requestId, int articleType, const std::string& articleText) {}
void tWrapper::historicalNews(int requestId, const std::string& time, const std::string& providerCode, const std::string& articleId, const std::string& headline) {}
void tWrapper::historicalNewsEnd(int requestId, bool hasMore) {}
void tWrapper::headTimestamp(int reqId, const std::string& headTimestamp) {}
void tWrapper::histogramData(int reqId, const HistogramDataVector& data) {}
void tWrapper::historicalDataUpdate(TickerId reqId, const Bar& bar) {}
void tWrapper::rerouteMktDataReq(int reqId, int conid, const std::string& exchange) {}
void tWrapper::rerouteMktDepthReq(int reqId, int conid, const std::string& exchange) {}
void tWrapper::marketRule(int marketRuleId, const std::vector<PriceIncrement> &priceIncrements) {}
void tWrapper::pnl(int reqId, double dailyPnL, double unrealizedPnL, double realizedPnL) {}
void tWrapper::pnlSingle(int reqId, Decimal pos, double dailyPnL, double unrealizedPnL, double realizedPnL, double value) {}
void tWrapper::historicalTicks(int reqId, const std::vector<HistoricalTick> &ticks, bool done) {}
void tWrapper::historicalTicksBidAsk(int reqId, const std::vector<HistoricalTickBidAsk> &ticks, bool done) {}
void tWrapper::historicalTicksLast(int reqId, const std::vector<HistoricalTickLast> &ticks, bool done) {}
void tWrapper::tickByTickAllLast(int reqId, int tickType, time_t time, double price, Decimal size, const TickAttribLast& tickAttribLast, const std::string& exchange, const std::string& specialConditions) {}
void tWrapper::tickByTickBidAsk(int reqId, time_t time, double bidPrice, double askPrice, Decimal bidSize, Decimal askSize, const TickAttribBidAsk& tickAttribBidAsk) {}
void tWrapper::tickByTickMidPoint(int reqId, time_t time, double midPoint) {}
void tWrapper::orderBound(long long orderId, int apiClientId, int apiOrderId) {}
void tWrapper::completedOrder(const Contract& contract, const Order& order, const OrderState& orderState) {}
void tWrapper::completedOrdersEnd() {}
void tWrapper::replaceFAEnd(int reqId, const std::string& text) {}
void tWrapper::wshMetaData(int reqId, const std::string& dataJson) {}
void tWrapper::wshEventData(int reqId, const std::string& dataJson) {}
void tWrapper::historicalSchedule(int reqId, const std::string& startDateTime, const std::string& endDateTime, const std::string& timeZone, const std::vector<HistoricalSession>& sessions) {}
void tWrapper::userInfo(int reqId, const std::string& whiteBrandingId) {}

