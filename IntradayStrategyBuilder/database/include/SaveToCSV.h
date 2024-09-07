#ifndef SAVETOCSV_H
#define SAVETOCSV_H

#include <iostream>
#include <filesystem>  
#include <fstream>    
#include <ctime>      
#include <string> 
#include <vector>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>

namespace fs = std::filesystem;

enum class DataType {
    Tick,
    FiveSec,
    OneMin,
    UnderlyingOneMinute,
    UnderlyingAverages,
    DailyAverages,
    News
};

struct DataPoint {
    std::string equity{""};
    int contractId{0};
    std::string contractType{""};
    DataType dataType;;
    std::string data{""};
};

class CSVFileSaver {
    public:
        CSVFileSaver();
        void start();
        void stop();

        static std::string valueToCSV(double value);

        void createDirectoriesAndFiles(const std::string& equity, int contractStrike, const std::string& contractType);
        // For data type, 1 will be ticks, 2 will be five sec data, 3 will be one min data
        void writeDataToFiles(const std::string& equity, int contractId, const std::string& contractType, 
            DataType dataType, const std::string& data);

        void checkDataQueues();
        void addDataToQueue(const std::string& equity, int contractId, const std::string& contractType, 
            DataType dataType, const std::string& data);

    private:
        std::thread dataQueueThread;
        std::mutex csvMtx;
        std::queue<DataPoint> dataQueue;

        bool endQueue = false;

        // File definitions
        std::string tickHeaders{""};
        std::string fiveSecHeaders{""};
        std::string oneMinHeaders{""};
        std::string underlyingOneMinHeaders{""};
        std::string underlyingAvgHeaders{""};
        std::string contractNewsHeaders{""};

};

#endif