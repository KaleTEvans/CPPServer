#include <iostream>
#include <thread>
#include <chrono>

#include "TwsWrapper.h"
#include "TwsApiDefs.h"
#include "SampleDataSubscriber.h"
#include "ContractDefs.h"

using namespace TwsApi;

int main() {
    std::shared_ptr<tWrapper> testClient = std::make_shared<tWrapper>();

    int clientId = 0;

	unsigned attempt = 0;
	printf( "Start of C++ Socket Client Test %u\n", attempt);

    testClient->connect("192.168.12.148", 7496, clientId);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    Subscriber testSubscriber(testClient);

    // Create contract
    Contract con;
    con.symbol = "SPX";
    con.secType = "IND";
    con.currency = "USD";
    con.exchange = "SMART";
    con.primaryExchange = "CBOE";

    testClient->startMsgProcessingThread();

    // Use reqContractDetails to get the contract ID
    // First add a req id to the list
    int req = testSubscriber.getContractData(ContractDefs::SPXInd());

    while (!testClient->checkEventCompleted(req)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    Contract con1;
    con1.symbol = "SPX";
    con1.secType = "OPT";
    con1.currency = "USD";
    con1.exchange = "SMART";
    con1.primaryExchange = "CBOE";
    con1.lastTradeDateOrContractMonth = "20240430";
    con1.strike = 5060;

    Contract con2 = con1;
    con1.right = "C";
    con2.right = "P";

    //testClient.reqRealTimeBars(con1, 5, "TRADES", true);
    //testClient.reqRealTimeBars(con2, 5, "TRADES", true);
    testClient->reqMktData(con2, "100,101,106,104,225,233,293,295,411", false, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    int chainID = 0;
    if (testSubscriber.lastPrice != 0) chainID = testSubscriber.getOptionsChain("SPX", "", "IND", ContractDefs::SPXConID());
    while (!testClient->checkEventCompleted(chainID)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    Contract news;
    news.symbol = "BZ:BZ_ALL";
    news.secType = "NEWS";
    news.exchange = "BZ";
    
    testClient->reqMktData(news, "mdoff,292", false, false);

    for (int i=0; i < 150; i++) {
        //testSubscriber.getTime();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Cancelling data for call" << std::endl;
    //testClient.cancelRealTimeBars(5111);
    for (int i=0; i < 5000; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    testClient->disconnect();

    return 0;
}

/*
Request output for Option Contract
TickOptionComputation. Ticker Id: 101, Type: 10, TickAttrib: 0,ImpliedVolatility: 0.348064, Delta: 0.476541, OptionPrice: 5.900000, pvDividend: 0.000000, Gamma: 0.024399,Vega: 0.186759, Theta: -5.900000, Underlying Price: 5108.990000
TickOptionComputation. Ticker Id: 101, Type: 11, TickAttrib: 0,ImpliedVolatility: 0.353414, Delta: 0.476915, OptionPrice: 6.100000, pvDividend: 0.000000, Gamma: 0.024031,Vega: 0.186770, Theta: -6.100000, Underlying Price: 5108.990000
TickOptionComputation. Ticker Id: 101, Type: 12, TickAttrib: 0,ImpliedVolatility: 0.364112, Delta: 0.477631, OptionPrice: 6.000000, pvDividend: 0.000000, Gamma: 0.023328,Vega: 0.186789, Theta: -6.000000, Underlying Price: 5108.990000
TickOptionComputation. Ticker Id: 101, Type: 13, TickAttrib: 0,ImpliedVolatility: 0.350800, Delta: 0.475281, OptionPrice: 6.053478, pvDividend: 0.000000, Gamma: 0.024205,Vega: 0.186721, Theta: -6.053478, Underlying Price: 5108.930000

Request output for SPX contract
Tick Size. Ticker Id: 101, Field: 5, Size: 
Tick Size. Ticker Id: 101, Field: 5, Size: 
Tick Size. Ticker Id: 101, Field: 0, Size: 
Tick Size. Ticker Id: 101, Field: 3, Size: 
Tick Size. Ticker Id: 101, Field: 29, Size: 3949864
Tick Size. Ticker Id: 101, Field: 30, Size: 5314554
Tick Size. Ticker Id: 101, Field: 29, Size: 3949958
Tick Size. Ticker Id: 101, Field: 30, Size: 5314772

Tick Generic. Ticker Id: 101, Type: 23, Value: 0.106777
Tick Generic. Ticker Id: 101, Type: 24, Value: 0.144352
Tick Size. Ticker Id: 101, Field: 29, Size: 4063246
Tick Size. Ticker Id: 101, Field: 30, Size: 5435346
Tick Generic. Ticker Id: 101, Type: 23, Value: 0.106775
Tick Generic. Ticker Id: 101, Type: 23, Value: 0.106803

Tick Size. Ticker Id: 101, Field: 29, Size: 4069834
Tick Size. Ticker Id: 101, Field: 30, Size: 5443470
Tick Size. Ticker Id: 101, Field: 27, Size: 18698086
Tick Size. Ticker Id: 101, Field: 28, Size: 37737470

Tick Price. Ticker Id: 101, Field: 4, Price: 5120.430000, CanAutoExecute: 0, PastLimit: 0, PreOpen: 0
Tick Size. Ticker Id: 101, Field: 5, Size: 
Tick Price. Ticker Id: 101, Field: 4, Price: 5120.500000, CanAutoExecute: 0, PastLimit: 0, PreOpen: 0
Tick Size. Ticker Id: 101, Field: 5, Size: 
Tick Price. Ticker Id: 101, Field: 1, Price: 0.000000, CanAutoExecute: 0, PastLimit: 0, PreOpen: 0
Tick Size. Ticker Id: 101, Field: 0, Size: 
Tick Price. Ticker Id: 101, Field: 2, Price: 0.000000, CanAutoExecute: 0, PastLimit: 0, PreOpen: 0
Tick Size. Ticker Id: 101, Field: 3, Size: 

=========
Ticks for SPX after market close 

Tick Price. Ticker Id: 1, Field: 1, Price: -1.000000, CanAutoExecute: 1, PastLimit: 0, PreOpen: 0
Tick Size. Ticker Id: 1, Field: 0, Size: 0
Tick Price. Ticker Id: 1, Field: 2, Price: -1.000000, CanAutoExecute: 1, PastLimit: 0, PreOpen: 0
Tick Size. Ticker Id: 1, Field: 3, Size: 0
Tick Price. Ticker Id: 1, Field: 4, Price: 5022.210000, CanAutoExecute: 0, PastLimit: 0, PreOpen: 0
Tick Size. Ticker Id: 1, Field: 5, Size: 
Tick Size. Ticker Id: 1, Field: 0, Size: 0
Tick Size. Ticker Id: 1, Field: 3, Size: 0
Tick Size. Ticker Id: 1, Field: 8, Size: 24600
Tick Price. Ticker Id: 1, Field: 6, Price: 5077.960000, CanAutoExecute: 0, PastLimit: 0, PreOpen: 0
Tick Price. Ticker Id: 1, Field: 7, Price: 5007.250000, CanAutoExecute: 0, PastLimit: 0, PreOpen: 0
Tick Price. Ticker Id: 1, Field: 9, Price: 5051.410000, CanAutoExecute: 0, PastLimit: 0, PreOpen: 0
Tick Size. Ticker Id: 1, Field: 29, Size: 3961530
Tick Size. Ticker Id: 1, Field: 30, Size: 4880340
Tick Size. Ticker Id: 1, Field: 27, Size: 19599996
Tick Size. Ticker Id: 1, Field: 28, Size: 38265290
Tick Generic. Ticker Id: 1, Type: 23, Value: 0.114762
Tick Generic. Ticker Id: 1, Type: 24, Value: 0.158317
Tick Size. Ticker Id: 1, Field: 29, Size: 4286204
Tick Size. Ticker Id: 1, Field: 30, Size: 5461824
Tick Generic. Ticker Id: 1, Type: 23, Value: 0.114374
========

News ID: BZ$1729eb9e
Time of Article: 1712952261000
Headline: On April 9, Leafly Holdings Received Notice From Nasdaq That Co No Longer Complies With Nasdaq's Requirements Contained In Nasdaq Listing Rule 5550
Extra DataA:800015:L:en:K:-0.81:C:0.8131997585296631

News ID: BZ$1729eba1
Time of Article: 1712952273000
Headline: Biden Expects Iran To Attack Israel 'Sooner Rather Than Later' &#x2014; IDF: 'We'll Know How To Deal With It'
Extra DataA:800015:L:en:K:n/a:C:0.8924752473831177

News ID: BZ$1729ec32
Time of Article: 1712952278000
Headline: On April 8, Aclarion Inc Received Notice Of Delisting Or Failure To Satisfy A Continued Listing Rule Or Standard From Nasdaq
Extra DataA:800015:L:en:K:-0.99:C:0.9896387457847595

News ID: BZ$1729ecdf
Time of Article: 1712952306000
Headline: Rail Vision Files For Mixed Shelf Offering Of Up To $100M
Extra DataA:800015:L:en:K:-0.99:C:0.9919460415840149

News ID: BZ$1729f04d
Time of Article: 1712952425000
Headline: Medical Properties Trust Sells Majority Interest In Utah Hospitals;  MPT Has Retained An ~25% Interest In The Venture And The Fund Purchased An ~75% Interest For $886M, Fully Validating MPT's Underwritten Lease Base Of ~$1.2B; Will Generate ~$1.1B Of Total Cash Proceeds
Extra DataA:800015:L:en:K:0.42:C:0.4189840257167816

News ID: BZ$172e1882
Time of Article: 1713205827000
Headline: $100 Invested In This Stock 5 Years Ago Would Be Worth $500 Today
Extra Data: A:800015:L:en:K:n/a:C:0.8727717995643616

News ID: BZ$172e1892
Time of Article: 1713205831000
Headline: How Is The Market Feeling About Schlumberger?
Extra Data: A:800015:L:en:K:n/a:C:0.879279613494873

News ID: BZ$172e1897
Time of Article: 1713205837000
Headline: S&P 500 Down Over 1%; US Retail Sales Increase 0.7% In March
Extra Data: A:800015:L:en:K:0.97:C:0.97

News ID: BZ$172e1904
Time of Article: 1713205860000
Headline: Cantor Fitzgerald Reiterates Overweight on Eli Lilly and Co, Maintains $815 Price Target
Extra Data: A:800015:L:en:K:0.97:C:0.97

News ID: BZ$172e1fdb
Time of Article: 1713206215000
Headline: Biohaven shares are trading lower after the company announced the presentatino of data at the 2024 American Academy of Neurology Annual Meeting this weekend.
Extra Data: A:800015:L:en:K:n/a:C:0.7615593671798706

*/