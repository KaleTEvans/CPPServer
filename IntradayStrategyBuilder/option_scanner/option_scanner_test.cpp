#include <iostream>
#include <set>

#include <ContractData.h>
#include <SaveToCSV.h>

int main() {
    std::shared_ptr<tWrapper> testClient = std::make_shared<tWrapper>();
    std::shared_ptr<CSVFileSaver> csv = std::make_shared<CSVFileSaver>();

    int clientId = 0;

	unsigned attempt = 0;
	printf( "Start of C++ Socket Client Test %u\n", attempt);

    testClient->connect("192.168.12.148", 7496, clientId);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    Contract con1;
    con1.symbol = "SPX";
    con1.secType = "OPT";
    con1.currency = "USD";
    con1.exchange = "SMART";
    con1.primaryExchange = "CBOE";
    con1.lastTradeDateOrContractMonth = "20240813";
    con1.strike = 5420;
    con1.right = "C";

    testClient->startMsgProcessingThread();
    csv->createDirectoriesAndFiles(con1.symbol, con1.strike, con1.right);
    csv->start();

    int mktDataId = testClient->reqMktData(con1, "100,101,106,104,225,232,233,293,294,295,411", false, false);
    int rtbId = testClient->reqRealTimeBars(con1, 5, "TRADES", true);

    ContractData cd(testClient, csv, mktDataId, rtbId, con1, 5);

    for (int i=0; i < 100000; i++) {
        //testSubscriber.getTime();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    //cd.saveData();

    testClient->disconnect();
    csv->stop();

    return 0;
}


/// Test Data Output

/*
-------------------------------------------------------------------------
1715111948731 | Tick Price. Ticker Id: 0, Field: Bid, Price: 0.400000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111948731 | Tick Size. Ticker Id: 0, Field: BidSize, Size: 25
-------------------------------------------------------------------------
1715111948832 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.45;1.0000000000000000;1715111947865;73854.0000000000000000;4.37885071;true
-------------------------------------------------------------------------
1715111948835 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.500000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111948835 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 14
1715111948835 | Tick Size. Ticker Id: 0, Field: LastTimestamp, Value: 1715111948
-------------------------------------------------------------------------
1715111949028 | Tick Price. Ticker Id: 0, Field: Last, Price: 0.350000, CanAutoExecute: 0,PastLimit: 0, PreOpen: 0
1715111949028 | Tick Size. Ticker Id: 0, Field: Volume, Size: 73836
1715111949028 | Tick Size. Ticker Id: 0, Field: LastTimestamp, Value: 1715111949
1715111949028 | TickOptionComputation. Ticker Id: 0, Type: ModelOptionComputation, TickAttrib: 0,ImpliedVolatility: 0.333026, Delta: 0.400538, OptionPrice: 2.150269, pvDividend: 0.000000, Gamma: 0.051282,Vega: 0.087459, Theta: -2.150269, Underlying Price: 5188.090000
-------------------------------------------------------------------------
1715111949129 | Tick Price. Ticker Id: 0, Field: Bid, Price: 0.400000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111949129 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 33
1715111949129 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.35;1.0000000000000000;1715111948163;73856.0000000000000000;4.37874161;true
-------------------------------------------------------------------------
1715111949228 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.40;1.0000000000000000;1715111948262;73857.0000000000000000;4.37868773;true
-------------------------------------------------------------------------
1715111949229 | Tick Price. Ticker Id: 0, Field: Last, Price: 0.400000, CanAutoExecute: 0,PastLimit: 0, PreOpen: 0
1715111949229 | Tick Size. Ticker Id: 0, Field: Volume, Size: 73837
-------------------------------------------------------------------------
1715111949730 | Tick Price. Ticker Id: 0, Field: Bid, Price: 0.350000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111949730 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 20
-------------------------------------------------------------------------
1715111950086 | TickOptionComputation. Ticker Id: 0, Type: ModelOptionComputation, TickAttrib: 0,ImpliedVolatility: 0.333026, Delta: 0.393376, OptionPrice: 2.094697, pvDividend: 0.000000, Gamma: 0.051035,Vega: 0.087033, Theta: -2.094697, Underlying Price: 5187.950000
-------------------------------------------------------------------------
1715111950131 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.450000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111950131 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 13
-------------------------------------------------------------------------
1715111950236 | Tick Price. Ticker Id: 0, Field: Last, Price: 0.350000, CanAutoExecute: 0,PastLimit: 0, PreOpen: 0
1715111950236 | Tick Size. Ticker Id: 0, Field: Volume, Size: 73838
1715111950236 | Tick Size. Ticker Id: 0, Field: LastTimestamp, Value: 1715111950
-------------------------------------------------------------------------
ID: 1 | Date: 20240507 14:59:05 | Time: 1715111945 Close: 0.4 | Volume: 11 | Traded Count: 11
--------------------------------------------------------------
1715111950329 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.35;1.0000000000000000;1715111949363;73859.0000000000000000;4.37857864;true
-------------------------------------------------------------------------
1715111950842 | TickOptionComputation. Ticker Id: 0, Type: ModelOptionComputation, TickAttrib: 0,ImpliedVolatility: 0.333026, Delta: 0.390826, OptionPrice: 2.075092, pvDividend: 0.000000, Gamma: 0.050943,Vega: 0.086875, Theta: -2.075092, Underlying Price: 5187.900000
-------------------------------------------------------------------------
1715111950893 | Tick Price. Ticker Id: 0, Field: Last, Price: 0.450000, CanAutoExecute: 0,PastLimit: 0, PreOpen: 0
1715111950893 | Tick Size. Ticker Id: 0, Field: Volume, Size: 73849
1715111950893 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.45;10.0000000000000000;1715111949928;73870.0000000000000000;4.37799228;false
-------------------------------------------------------------------------
1715111950896 | Tick Price. Ticker Id: 0, Field: Bid, Price: 0.350000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111950896 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 46
-------------------------------------------------------------------------
1715111951032 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.400000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111951032 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 10
-------------------------------------------------------------------------
1715111951134 | Tick Size. Ticker Id: 0, Field: LastTimestamp, Value: 1715111951
-------------------------------------------------------------------------
1715111951136 | Tick Price. Ticker Id: 0, Field: Bid, Price: 0.300000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111951136 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 9
-------------------------------------------------------------------------
1715111951237 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.450000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111951237 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 26
1715111951237 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.40;9.0000000000000000;1715111950272;73882.0000000000000000;4.37734482;false
-------------------------------------------------------------------------
1715111951641 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.400000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111951641 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 1
-------------------------------------------------------------------------
1715111951835 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.450000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111951835 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 4
1715111951835 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.40;2.0000000000000000;1715111950869;73884.0000000000000000;4.37723729;false
-------------------------------------------------------------------------
1715111951935 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.500000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111951935 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 59
1715111951935 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.45;4.0000000000000000;1715111950970;73888.0000000000000000;4.37702469;false
-------------------------------------------------------------------------
1715111952036 | Tick Price. Ticker Id: 0, Field: Bid, Price: 0.350000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111952036 | Tick Size. Ticker Id: 0, Field: BidSize, Size: 51
1715111952036 | TickOptionComputation. Ticker Id: 0, Type: LastOptionComputation, TickAttrib: 0,ImpliedVolatility: 0.140523, Delta: 0.252161, OptionPrice: 0.450000, pvDividend: 0.000000, Gamma: 0.100390,Vega: 0.072237, Theta: -0.450000, Underlying Price: 5187.870000
-------------------------------------------------------------------------
1715111952135 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.40;3.0000000000000000;1715111951169;73891.0000000000000000;4.37686362;true
-------------------------------------------------------------------------
1715111952236 | Tick Price. Ticker Id: 0, Field: Bid, Price: 0.400000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111952236 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 42
-------------------------------------------------------------------------
1715111952537 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 73
-------------------------------------------------------------------------
1715111952734 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.40;2.0000000000000000;1715111951768;73893.0000000000000000;4.37675599;true
-------------------------------------------------------------------------
1715111952740 | Tick Price. Ticker Id: 0, Field: Last, Price: 0.400000, CanAutoExecute: 0,PastLimit: 0, PreOpen: 0
1715111952740 | Tick Size. Ticker Id: 0, Field: Volume, Size: 73858
1715111952740 | Tick Size. Ticker Id: 0, Field: LastTimestamp, Value: 1715111952
-------------------------------------------------------------------------
1715111952835 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.450000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111952835 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 14
1715111952835 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.40;3.0000000000000000;1715111951869;73896.0000000000000000;4.37659454;true
-------------------------------------------------------------------------
1715111952934 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.40;10.0000000000000000;1715111951969;73906.0000000000000000;4.37605648;true
-------------------------------------------------------------------------
1715111952936 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.400000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111952936 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 53
-------------------------------------------------------------------------
1715111953035 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.40;6.0000000000000000;1715111952069;73912.0000000000000000;4.37573209;false
-------------------------------------------------------------------------
1715111953086 | TickOptionComputation. Ticker Id: 0, Type: ModelOptionComputation, TickAttrib: 0,ImpliedVolatility: 0.333026, Delta: 0.394397, OptionPrice: 2.102574, pvDividend: 0.000000, Gamma: 0.051072,Vega: 0.087096, Theta: -2.102574, Underlying Price: 5187.970000
-------------------------------------------------------------------------
1715111953339 | Tick Price. Ticker Id: 0, Field: Bid, Price: 0.350000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111953339 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 29
1715111953339 | Tick Size. Ticker Id: 0, Field: LastTimestamp, Value: 1715111953
-------------------------------------------------------------------------
1715111953544 | Tick Price. Ticker Id: 0, Field: Last, Price: 0.400000, CanAutoExecute: 0,PastLimit: 0, PreOpen: 0
1715111953544 | Tick Size. Ticker Id: 0, Field: Volume, Size: 73861
1715111953544 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.40;2.0000000000000000;1715111952570;73916.0000000000000000;4.37551491;false
-------------------------------------------------------------------------
1715111953635 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.40;29.0000000000000000;1715111952670;73945.0000000000000000;4.37395578;true
-------------------------------------------------------------------------
1715111953641 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.450000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111953641 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 10
-------------------------------------------------------------------------
1715111953735 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.400000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111953735 | Tick Size. Ticker Id: 0, Field: BidSize, Size: 15
-------------------------------------------------------------------------
1715111953835 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.40;20.0000000000000000;1715111952869;73965.0000000000000000;4.37287447;false
-------------------------------------------------------------------------
1715111953838 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.450000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111953838 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 43
-------------------------------------------------------------------------
1715111953896 | TickOptionComputation. Ticker Id: 0, Type: BidOptionComputation, TickAttrib: 0,ImpliedVolatility: 0.328004, Delta: 0.391258, OptionPrice: 0.350000, pvDividend: 0.000000, Gamma: 0.051739,Vega: 0.086902, Theta: -0.350000, Underlying Price: 5187.940000
-------------------------------------------------------------------------
1715111953897 | TickOptionComputation. Ticker Id: 0, Type: ModelOptionComputation, TickAttrib: 0,ImpliedVolatility: 0.333026, Delta: 0.392865, OptionPrice: 2.090766, pvDividend: 0.000000, Gamma: 0.051017,Vega: 0.087002, Theta: -2.090766, Underlying Price: 5187.940000
-------------------------------------------------------------------------
1715111954440 | Tick Price. Ticker Id: 0, Field: Last, Price: 0.350000, CanAutoExecute: 0,PastLimit: 0, PreOpen: 0
1715111954440 | Tick Size. Ticker Id: 0, Field: Volume, Size: 73902
1715111954440 | Tick Size. Ticker Id: 0, Field: LastTimestamp, Value: 1715111954
-------------------------------------------------------------------------
1715111954642 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.400000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111954642 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 2
1715111954642 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.35;1.0000000000000000;1715111953676;73967.0000000000000000;4.37276569;true
-------------------------------------------------------------------------
1715111954740 | Tick Price. Ticker Id: 0, Field: Bid, Price: 0.300000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111954740 | Tick Size. Ticker Id: 0, Field: BidSize, Size: 100
-------------------------------------------------------------------------
1715111954940 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.350000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111954940 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 1
-------------------------------------------------------------------------
1715111955040 | TickOptionComputation. Ticker Id: 0, Type: ModelOptionComputation, TickAttrib: 0,ImpliedVolatility: 0.333026, Delta: 0.390826, OptionPrice: 2.075092, pvDividend: 0.000000, Gamma: 0.050943,Vega: 0.086875, Theta: -2.075092, Underlying Price: 5187.900000
-------------------------------------------------------------------------
ID: 1 | Date: 20240507 14:59:10 | Time: 1715111950 Close: 0.35 | Volume: 65 | Traded Count: 16
--------------------------------------------------------------
1715111955341 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.35;2.0000000000000000;1715111954374;73969.0000000000000000;4.37265692;false
-------------------------------------------------------------------------
1715111955342 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.400000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111955342 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 44
-------------------------------------------------------------------------
1715111955445 | Tick Size. Ticker Id: 0, Field: Volume, Size: 73903
1715111955445 | Tick Size. Ticker Id: 0, Field: LastTimestamp, Value: 1715111955
-------------------------------------------------------------------------
1715111955641 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.30;6.0000000000000000;1715111954676;73975.0000000000000000;4.3723266;false
-------------------------------------------------------------------------
1715111955642 | Tick Price. Ticker Id: 0, Field: Last, Price: 0.300000, CanAutoExecute: 0,PastLimit: 0, PreOpen: 0
1715111955642 | Tick Size. Ticker Id: 0, Field: Volume, Size: 73904
-------------------------------------------------------------------------
1715111955739 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.40;5.0000000000000000;1715111954774;73980.0000000000000000;4.37205812;false
-------------------------------------------------------------------------
1715111955740 | Tick Price. Ticker Id: 0, Field: Last, Price: 0.400000, CanAutoExecute: 0,PastLimit: 0, PreOpen: 0
1715111955740 | Tick Size. Ticker Id: 0, Field: Volume, Size: 73906
-------------------------------------------------------------------------
1715111955840 | TickOptionComputation. Ticker Id: 0, Type: ModelOptionComputation, TickAttrib: 0,ImpliedVolatility: 0.333026, Delta: 0.376134, OptionPrice: 1.963889, pvDividend: 0.000000, Gamma: 0.050368,Vega: 0.085884, Theta: -1.963889, Underlying Price: 5187.610000
-------------------------------------------------------------------------
1715111955841 | TickOptionComputation. Ticker Id: 0, Type: LastOptionComputation, TickAttrib: 0,ImpliedVolatility: 0.140523, Delta: 0.226786, OptionPrice: 0.400000, pvDividend: 0.000000, Gamma: 0.094743,Vega: 0.068167, Theta: -0.400000, Underlying Price: 5187.610000
-------------------------------------------------------------------------
1715111956039 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.350000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111956039 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 1
-------------------------------------------------------------------------
1715111956140 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.35;5.0000000000000000;1715111955174;73985.0000000000000000;4.37178712;false
-------------------------------------------------------------------------
1715111956143 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.400000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111956143 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 44
-------------------------------------------------------------------------
1715111956144 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 44
-------------------------------------------------------------------------
1715111956341 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.35;17.0000000000000000;1715111955375;74002.0000000000000000;4.37086754;false
-------------------------------------------------------------------------
1715111956345 | Tick Price. Ticker Id: 0, Field: Last, Price: 0.350000, CanAutoExecute: 0,PastLimit: 0, PreOpen: 0
1715111956345 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 78
1715111956345 | Tick Size. Ticker Id: 0, Field: LastTimestamp, Value: 1715111956
-------------------------------------------------------------------------
1715111956440 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.350000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111956440 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 2
1715111956440 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.30;1.0000000000000000;1715111955475;74003.0000000000000000;4.37081253;true
-------------------------------------------------------------------------
1715111956642 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.35;1.0000000000000000;1715111955674;74004.0000000000000000;4.3707582;true
-------------------------------------------------------------------------
1715111956645 | Tick Price. Ticker Id: 0, Field: Last, Price: 0.350000, CanAutoExecute: 0,PastLimit: 0, PreOpen: 0
1715111956645 | Tick Size. Ticker Id: 0, Field: Volume, Size: 73909
-------------------------------------------------------------------------
1715111956946 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 27
1715111956946 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.30;1.0000000000000000;1715111955980;74005.0000000000000000;4.37070347;true
-------------------------------------------------------------------------
1715111957047 | TickOptionComputation. Ticker Id: 0, Type: ModelOptionComputation, TickAttrib: 0,ImpliedVolatility: 0.333026, Delta: 0.365106, OptionPrice: 1.882356, pvDividend: 0.000000, Gamma: 0.049887,Vega: 0.085057, Theta: -1.882356, Underlying Price: 5187.390000
-------------------------------------------------------------------------
1715111957146 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.400000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111957146 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 143
1715111957146 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.35;1.0000000000000000;1715111956177;74006.0000000000000000;4.37064914;true
-------------------------------------------------------------------------
1715111957246 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.350000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111957246 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 1
-------------------------------------------------------------------------
1715111957444 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.30;10.0000000000000000;1715111956479;74016.0000000000000000;4.37010187;true
-------------------------------------------------------------------------
1715111957548 | Tick Price. Ticker Id: 0, Field: Last, Price: 0.300000, CanAutoExecute: 0,PastLimit: 0, PreOpen: 0
1715111957548 | Tick Size. Ticker Id: 0, Field: Volume, Size: 73910
1715111957548 | Tick Size. Ticker Id: 0, Field: LastTimestamp, Value: 1715111957
-------------------------------------------------------------------------
1715111957646 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.30;1.0000000000000000;1715111956680;74022.0000000000000000;4.36977196;true
-------------------------------------------------------------------------
1715111957901 | TickOptionComputation. Ticker Id: 0, Type: ModelOptionComputation, TickAttrib: 0,ImpliedVolatility: 0.333026, Delta: 0.360625, OptionPrice: 1.849698, pvDividend: 0.000000, Gamma: 0.049679,Vega: 0.084700, Theta: -1.849698, Underlying Price: 5187.300000
-------------------------------------------------------------------------
1715111957904 | TickOptionComputation. Ticker Id: 0, Type: LastOptionComputation, TickAttrib: 0,ImpliedVolatility: 0.140523, Delta: 0.198507, OptionPrice: 0.300000, pvDividend: 0.000000, Gamma: 0.087655,Vega: 0.063060, Theta: -0.300000, Underlying Price: 5187.300000
-------------------------------------------------------------------------
1715111958145 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.30;10.0000000000000000;1715111957179;74032.0000000000000000;4.36922493;true
-------------------------------------------------------------------------
1715111958252 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 5
-------------------------------------------------------------------------
1715111958347 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.35;13.0000000000000000;1715111957381;74045.0000000000000000;4.3685186;false
-------------------------------------------------------------------------
1715111958348 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.400000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111958348 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 56
1715111958348 | Tick Size. Ticker Id: 0, Field: LastTimestamp, Value: 1715111958
-------------------------------------------------------------------------
1715111958449 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 3
-------------------------------------------------------------------------
1715111958646 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.350000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111958646 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 10
1715111958646 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.30;1.0000000000000000;1715111957680;74046.0000000000000000;4.36846366;true
-------------------------------------------------------------------------
1715111958846 | Tick Price. Ticker Id: 0, Field: Last, Price: 0.350000, CanAutoExecute: 0,PastLimit: 0, PreOpen: 0
1715111958846 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 15
1715111958846 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.35;1.0000000000000000;1715111957879;74047.0000000000000000;4.36840939;true
-------------------------------------------------------------------------
1715111958956 | Tick Price. Ticker Id: 0, Field: Bid, Price: 0.250000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111958956 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 38
-------------------------------------------------------------------------
1715111959046 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.30;2.0000000000000000;1715111958080;74049.0000000000000000;4.36830018;false
1715111959046 | TickOptionComputation. Ticker Id: 0, Type: ModelOptionComputation, TickAttrib: 0,ImpliedVolatility: 0.333026, Delta: 0.368603, OptionPrice: 1.908035, pvDividend: 0.000000, Gamma: 0.050044,Vega: 0.085327, Theta: -1.908035, Underlying Price: 5187.460000
-------------------------------------------------------------------------
1715111959150 | Tick Price. Ticker Id: 0, Field: Bid, Price: 0.300000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111959150 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 82
-------------------------------------------------------------------------
1715111959250 | Tick Size. Ticker Id: 0, Field: RtVolume, Value: 0.30;1.0000000000000000;1715111959139;74050.0000000000000000;4.36824524;true
-------------------------------------------------------------------------
1715111959251 | Tick Price. Ticker Id: 0, Field: Bid, Price: 0.250000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111959251 | Tick Size. Ticker Id: 0, Field: BidSize, Size: 26
1715111959251 | Tick Size. Ticker Id: 0, Field: LastTimestamp, Value: 1715111959
-------------------------------------------------------------------------
1715111959755 | Tick Price. Ticker Id: 0, Field: Ask, Price: 0.300000, CanAutoExecute: 1,PastLimit: 0, PreOpen: 0
1715111959755 | Tick Size. Ticker Id: 0, Field: BidSize, Size: 23
-------------------------------------------------------------------------
1715111959756 | Tick Size. Ticker Id: 0, Field: AskSize, Size: 1
-------------------------------------------------------------------------

*/