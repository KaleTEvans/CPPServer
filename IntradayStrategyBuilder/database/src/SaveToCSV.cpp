#include "SaveToCSV.h"

void CSVFileSaver::start() {
    // Start a thread to save data to csv files
    dataQueueThread = std::thread([this]() {
        checkDataQueues();
    });
}

void CSVFileSaver::stop() {
    endQueue = true;
    if (dataQueueThread.joinable()) dataQueueThread.join();
}

void CSVFileSaver::createDirectoriesAndFiles(const std::string& equity, int contractStrike, const std::string& contractType) {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    std::string parentDirName = std::to_string(1900 + localTime->tm_year) + "_" +
                                std::to_string(1 + localTime->tm_mon) + "_" + equity;
    std::string dirName = std::to_string(localTime->tm_mday) + "_" + equity;

    fs::path fullPath = fs::path(parentDirName) / dirName;

    if (!fs::exists(parentDirName)) {
        fs::create_directory(parentDirName);
    }

    if (!fs::exists(fullPath)) {
        fs::create_directory(fullPath);
    }

    std::string ticksFile = std::to_string(contractStrike) + "_" + contractType + "_Ticks.csv";
    std::string fiveSecFile = std::to_string(contractStrike) + "_" + contractType + "_Five_Sec.csv";
    std::string oneMinFile = std::to_string(contractStrike) + "_" + contractType + "_One_Min.csv";

    std::ofstream(fullPath / ticksFile);
    std::ofstream(fullPath / fiveSecFile);
    std::ofstream(fullPath / oneMinFile);

    std::string tickDataFile = (fullPath / (std::to_string(contractStrike) + "_" + contractType + "_Ticks.csv")).string();
    std::string tickHeaders = "Timestamp,BidPrice,BidSize,AskPrice,AskSize,LastPrice,MarkPrice,Volume,ImpliedVol,"
                    "Delta,Gamma,Vega,Theta,UnderlyingPrice,PriceOfSale,QuantityOfSale,TotalVol,VWAP,"
                    "FilledBySingleMM,RTM\n";

    std::ofstream tickFileStream(tickDataFile, std::ios::app);
    if (tickFileStream.is_open()) {
        tickFileStream << tickHeaders;
    }
    tickFileStream.close();

    std::string fiveSecDataFile = (fullPath / (std::to_string(contractStrike) + "_" + contractType + "_Five_Sec.csv")).string();
    std::string fiveSecHeaders = "TimeStamp,Open,Close,High,Low,Volume,Count,RTM\n";

    std::ofstream fiveSecFileStream(fiveSecDataFile, std::ios::app);
    if (fiveSecFileStream.is_open()) {
        fiveSecFileStream << fiveSecHeaders;
    }
    fiveSecFileStream.close();

    std::string oneMinDataFile = (fullPath / (std::to_string(contractStrike) + "_" + contractType + "_One_Min.csv")).string();
    std::string oneMinHeaders = "TimeStamp,Open,Close,High,Low,Volume,Count,ImpliedVol,Delta,Gamma,Vega,Theta,UnderlyingPrice,TotalVol,RTM\n";

    std::ofstream oneMinFileStream(oneMinDataFile, std::ios::app);
    if (oneMinFileStream.is_open()) {
        oneMinFileStream << oneMinHeaders;
    }
    oneMinFileStream.close();

    std::cout << "Directory and files created at: " << fullPath << std::endl;
}

void CSVFileSaver::writeDataToFiles(const std::string& equity, int contractId, const std::string& contractType, 
    int dataType, const std::string& data) {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    std::string parentDirName = std::to_string(1900 + localTime->tm_year) + "_" +
                                std::to_string(1 + localTime->tm_mon) + "_" + equity;
    std::string dirName = std::to_string(localTime->tm_mday) + "_" + equity;

    fs::path fullPath = fs::path(parentDirName) / dirName;
    std::string dataFile;
    std::string headers;

    switch (dataType)
    {
    case 1:
        dataFile = (fullPath / (std::to_string(contractId) + "_" + contractType + "_Ticks.csv")).string();
        break;
    case 2:
        dataFile = (fullPath / (std::to_string(contractId) + "_" + contractType + "_Five_Sec.csv")).string();
        break;
    case 3:
        dataFile = (fullPath / (std::to_string(contractId) + "_" + contractType + "_One_Min.csv")).string();
        break;
    
    default:
        std::cout << "Invalid datatype given to writeCsv" << std::endl;
        break;
    }

    std::ofstream dataFileStream(dataFile, std::ios::app);
    if (dataFileStream.is_open()) {
        dataFileStream << data;
    }
    dataFileStream.close();
}

void CSVFileSaver::checkDataQueues() {
    while (!endQueue) {
        while (!dataQueue.empty()) {
            writeDataToFiles(
                dataQueue.front().equity,
                dataQueue.front().contractId,
                dataQueue.front().contractType,
                dataQueue.front().dataType,
                dataQueue.front().data
            );
            dataQueue.pop();
        }
    }
}

void CSVFileSaver::addTicksToQueue(const std::string& equity, int contractId, const std::string& contractType, const std::string& data) {
    DataPoint dp;
    dp.equity = equity;
    dp.contractId = contractId;
    dp.contractType = contractType;
    dp.dataType = 1;
    dp.data = data;

    dataQueue.push(dp);
}

void CSVFileSaver::addFiveSecDataToQueue(const std::string& equity, int contractId, const std::string& contractType, const std::string& data) {
    DataPoint dp;
    dp.equity = equity;
    dp.contractId = contractId;
    dp.contractType = contractType;
    dp.dataType = 2;
    dp.data = data;

    dataQueue.push(dp);
}

void CSVFileSaver::addOneMinDataToQueue(const std::string& equity, int contractId, const std::string& contractType, const std::string& data) {
    DataPoint dp;
    dp.equity = equity;
    dp.contractId = contractId;
    dp.contractType = contractType;
    dp.dataType = 3;
    dp.data = data;

    dataQueue.push(dp);
}