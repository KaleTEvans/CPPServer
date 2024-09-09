#include "SaveToCSV.h"

CSVFileSaver::CSVFileSaver() {
    tickHeaders = "Timestamp,BidPrice,BidSize,AskPrice,AskSize,LastPrice,MarkPrice,Volume,ImpliedVol,"
                    "Delta,Gamma,Vega,Theta,UnderlyingPrice,PriceOfSale,QuantityOfSale,TotalVol,VWAP,"
                    "RTM,FilledBySingleMM\n";

    fiveSecHeaders = "TimeStamp,Open,Close,High,Low,Volume,Count,RTM\n";

    oneMinHeaders = "TimeStamp,Open,Close,High,Low,Volume,Count,ImpliedVol,Delta,"
                    "Gamma,Vega,Theta,UnderlyingPrice,TotalVol,RTM\n";

    underlyingOneMinHeaders = "TimeStamp,Open,High,Low,Close,Volume,DailyHigh,DailyLow,DailyVolume,"
                                "TotalCallVolume,TotalPutVolume,IndexFuturePremium,TotalTradeCount,"
                                "OneMinuteTradeRate,RealTimeVol_30Day,OptionIV_30Day,OptionVol_30Day,"
                                "CallOpenInterest,PutOpenInterest,FuturesOpenInterest\n";

    underlyingAvgHeaders = "13WeekLow,13WeekHigh,26WeekLow,26WeekHigh,52WeekLow,52WeekHigh,AverageVolume_90Day\n";

    contractNewsHeaders = "TimeStamp,ArticleId,Headline,SentimentScore,Price\n";

    largeOrderAlertHeaders = "TimeStamp,Equity,ContractStrike,Right,PriceOfSale,QuantityOfSale,TotalSale,"
                                "TotalOptionVolume,VWAP,CurrentAsk,CurrentBid\n";
}

void CSVFileSaver::start() {
    // Start a thread to save data to csv files
    dataQueueThread = std::thread([this]() {
        checkDataQueues();
    });
}

void CSVFileSaver::stop() {
    endQueue = true;

    if (dataQueueThread.joinable()) dataQueueThread.join();
    std::cout << "CSV File Saver has stopped" << std::endl;
}

std::string CSVFileSaver::valueToCSV(double value) {
    return (value == -1 || value == -100) ? "" : std::to_string(value);
}

void CSVFileSaver::createDirectoriesAndFiles(const std::string& equity, int contractStrike, const std::string& contractType) {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    std::string baseDirName = "SavedData";
    std::string parentDirName = std::to_string(1900 + localTime->tm_year) + "_" +
                                std::to_string(1 + localTime->tm_mon) + "_" + equity;
    std::string dirName = std::to_string(localTime->tm_mday) + "_" + equity;

    fs::path fullPath = fs::path(baseDirName) / parentDirName / dirName;

    if (!fs::exists(fullPath)) {
        fs::create_directories(fullPath);
    }

    std::string ticksFile = std::to_string(contractStrike) + "_" + contractType + "_Ticks.csv";
    std::string fiveSecFile = std::to_string(contractStrike) + "_" + contractType + "_Five_Sec.csv";
    std::string oneMinFile = std::to_string(contractStrike) + "_" + contractType + "_One_Min.csv";

    // Use a contract strike of 0 for underlying data
    if (contractStrike == 0) {
        std::string underlyingOneMinFile = equity + "_One_Min.csv";
        std::string dailyStats = equity + "_Daily_Data.csv";
        std::string newsData = equity + "_News_Data.csv";
        std::string largeOrderData = equity + "_Large_Orders.csv";

        std::ofstream(fullPath / underlyingOneMinFile);
        std::ofstream(fullPath / dailyStats);
        std::ofstream(fullPath / newsData);
        std::ofstream(fullPath / largeOrderData);

        std::string underlyingOneMinDataFile = (fullPath / (underlyingOneMinFile)).string();
        std::string dailyDataFile = (fullPath / (dailyStats)).string();
        std::string newsDataFile = (fullPath / (newsData)).string();
        std::string largeOrderDataFile = (fullPath / (largeOrderData)).string();

        if (!fs::exists(underlyingOneMinFile)) {
            std::ofstream underlyingOneMinFileStream(underlyingOneMinDataFile, std::ios::app);
            if (underlyingOneMinFileStream.is_open()) {
                underlyingOneMinFileStream << underlyingOneMinHeaders;
            }
            underlyingOneMinFileStream.close();
        }
        if (!fs::exists(dailyDataFile)) {
            std::ofstream dailyStatsFileStream(dailyDataFile, std::ios::app);
            if (dailyStatsFileStream.is_open()) {
                dailyStatsFileStream << underlyingAvgHeaders;
            }
            dailyStatsFileStream.close();
        }
        if (!fs::exists(newsDataFile)) {
            std::ofstream newsDataFileStream(newsDataFile, std::ios::app);
            if (newsDataFileStream.is_open()) {
                newsDataFileStream << contractNewsHeaders;
            }
            newsDataFileStream.close();
        }
        if (!fs::exists(largeOrderDataFile)) {
            std::ofstream largeOrderDataFileStream(largeOrderDataFile, std::ios::app);
            if (largeOrderDataFileStream.is_open()) {
                largeOrderDataFileStream << largeOrderAlertHeaders;
            }
            largeOrderDataFileStream.close();
        }

    } else {
        std::ofstream(fullPath / ticksFile);
        std::ofstream(fullPath / fiveSecFile);
        std::ofstream(fullPath / oneMinFile);

        std::string tickDataFile = (fullPath / (std::to_string(contractStrike) + "_" + contractType + "_Ticks.csv")).string();

        if (!fs::exists(ticksFile)) {
            std::ofstream tickFileStream(tickDataFile, std::ios::app);
            if (tickFileStream.is_open()) {
                tickFileStream << tickHeaders;
            }
            tickFileStream.close();
        }

        std::string fiveSecDataFile = (fullPath / (std::to_string(contractStrike) + "_" + contractType + "_Five_Sec.csv")).string();

        if (!fs::exists(fiveSecFile)) {
            std::ofstream fiveSecFileStream(fiveSecDataFile, std::ios::app);
            if (fiveSecFileStream.is_open()) {
                fiveSecFileStream << fiveSecHeaders;
            }
            fiveSecFileStream.close();
        }

        std::string oneMinDataFile = (fullPath / (std::to_string(contractStrike) + "_" + contractType + "_One_Min.csv")).string();

        if (!fs::exists(oneMinFile)) {
            std::ofstream oneMinFileStream(oneMinDataFile, std::ios::app);
            if (oneMinFileStream.is_open()) {
                oneMinFileStream << oneMinHeaders;
            }
            oneMinFileStream.close();
        }
    }

    std::cout << "Directory and files created at: " << fullPath << std::endl;
}

void CSVFileSaver::writeDataToFiles(const std::string& equity, int contractId, const std::string& contractType, 
    DataType dataType, const std::string& data) {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    std::string baseDirName = "SavedData";
    std::string parentDirName = std::to_string(1900 + localTime->tm_year) + "_" +
                                std::to_string(1 + localTime->tm_mon) + "_" + equity;
    std::string dirName = std::to_string(localTime->tm_mday) + "_" + equity;

    fs::path fullPath = fs::path(baseDirName) / parentDirName / dirName;

    std::string dataFile;
    std::string headers;

    switch (dataType)
    {
    case DataType::Tick:
        dataFile = (fullPath / (std::to_string(contractId) + "_" + contractType + "_Ticks.csv")).string();
        break;
    case DataType::FiveSec:
        dataFile = (fullPath / (std::to_string(contractId) + "_" + contractType + "_Five_Sec.csv")).string();
        break;
    case DataType::OneMin:
        dataFile = (fullPath / (std::to_string(contractId) + "_" + contractType + "_One_Min.csv")).string();
        break;
    case DataType::UnderlyingOneMinute:
        dataFile = (fullPath / (equity + "_One_Min.csv")).string();
        break;
    case DataType::UnderlyingAverages:
        dataFile = (fullPath / (equity + "_Daily_Data.csv")).string();  
        break;
    case DataType::News:
        dataFile = (fullPath / (equity + "_News_Data.csv")).string();
        break;
    case DataType::LargeOrderAlert:
        dataFile = (fullPath / (equity + "_Large_Orders.csv")).string();
        break;
    
    default:
        std::cout << "Invalid datatype given to writeCsv" << std::endl;
        std::cout << data << std::endl;
        break;
    }

    std::ofstream dataFileStream(dataFile, std::ios::app);
    if (dataFileStream.is_open()) {
        dataFileStream << data;
    }
    dataFileStream.close();
}

void CSVFileSaver::checkDataQueues() {
    while (true) {
        while (!dataQueue.empty()) {
            std::lock_guard<std::mutex> lock(csvMtx);
            writeDataToFiles(
                dataQueue.front().equity,
                dataQueue.front().contractId,
                dataQueue.front().contractType,
                dataQueue.front().dataType,
                dataQueue.front().data
            );

            dataQueue.pop();
        }

        if (endQueue) {
            std::unique_lock<std::mutex> relock(csvMtx);
            if (dataQueue.empty()) break;
        }
    }
}

void CSVFileSaver::addDataToQueue(const std::string& equity, int contractId, const std::string& contractType, 
    DataType dataType, const std::string& data) {
    std::lock_guard<std::mutex> lock(csvMtx);
    DataPoint dp;
    dp.equity = equity;
    dp.contractId = contractId;
    dp.contractType = contractType;
    dp.dataType = dataType;
    dp.data = data;

    dataQueue.push(dp);
}