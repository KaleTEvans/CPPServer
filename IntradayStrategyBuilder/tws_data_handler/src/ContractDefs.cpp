#include "ContractDefs.h"

#include "Contract.h"
#include "Enumerations.h"

int ContractDefs::getConId(const std::string& symbol) {
    if (symbol == "SPX") return 416904;
    else if (symbol == "SPY") return 756733;
    else if (symbol == "AAPL") return 265598;

    else {
        std::cout << "Contract ID not found" << std::endl;
        return 0;
    }
}

Contract ContractDefs::SPXInd() {
    Contract contract;
    contract.symbol = "SPX";
    contract.secType = "IND";
    contract.currency = "USD";
    contract.exchange = "SMART";
    contract.primaryExchange = "CBOE";
    //contract.conId = 416904;
    return contract;
}

Contract ContractDefs::BZBroadTape() {
    Contract news;
    news.symbol = "BZ:BZ_ALL";
    news.secType = "NEWS";
    news.exchange = "BZ";

    return news;
}

Contract ContractDefs::SPXOpt(const std::string& expDate, const std::string& right, int strike) {
    Contract contract;
    contract.symbol = "SPX";
    contract.secType = "OPT";
    contract.currency = "USD";
    contract.exchange = "SMART";
    contract.primaryExchange = "CBOE";
    contract.lastTradeDateOrContractMonth = expDate;
    contract.right = right;
    contract.strike = strike;

    return contract;
}

Contract ContractDefs::SPXOpt0DTE(const std::string& right, int strike) {
    Contract contract;
    contract.symbol = "SPX";
    contract.secType = "OPT";
    contract.currency = "USD";
    contract.exchange = "SMART";
    contract.primaryExchange = "CBOE";

    std::time_t tmNow;
	tmNow = time(NULL);
	struct tm t = *localtime(&tmNow);
    std::string month = std::to_string(t.tm_mon + 1);
    if (month.size() == 1) month = "0" + month;
    std::string day = std::to_string(t.tm_mday);
    if (day.size() == 1) day = "0" + day;
    std::string date = std::to_string(t.tm_year + 1900) + month + day;
	contract.lastTradeDateOrContractMonth = date;

    contract.right = right;
    contract.strike = strike;

    return contract;
}

Contract ContractDefs::SPY() {
    Contract contract;
	contract.symbol = "SPY";
	contract.secType = "STK";
	contract.currency = "USD";
	contract.exchange = "SMART";
	return contract;
}

Contract ContractDefs::QQQ() {
    Contract contract;
	contract.symbol = "QQQ";
	contract.secType = "STK";
	contract.currency = "USD";
	contract.exchange = "SMART";
	return contract;
}

Contract ContractDefs::VIX() {
    Contract contract;
	contract.symbol = "VIX";
	contract.secType = "IND";
	contract.currency = "USD";
	contract.exchange = "SMART";
	return contract;
}

Contract ContractDefs::AAPL() {
    Contract contract;
    contract.symbol = "AAPL";
    contract.secType = "STK";
    contract.currency = "USD";
    contract.exchange = "SMART";
    return contract;
}

Contract ContractDefs::emptyContract() {
    Contract contract;
    contract.symbol = "";
    contract.strike = 0;
    return contract;
}