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

void tWrapper::processMessages() {
    time_t now = time(NULL);

    m_osSignal.waitForSignal();
	errno = 0;
	m_pReader->processMsgs();
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
	}
	else
		printf( "Cannot connect to %s:%d clientId:%d\n", m_pClient->host().c_str(), m_pClient->port(), clientId);

	return bRes;
}

void tWrapper::disconnect() const {
	m_pClient->eDisconnect();

	printf ( "Disconnected\n");
}

bool tWrapper::isConnected() const {
	return m_pClient->isConnected();
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
            std::cout << i;
        }
        std::cout << std::endl;
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
std::pair<TickerId, double> tWrapper::getLastPrice(TickerId reqId, double price) { return {reqId, price}; }