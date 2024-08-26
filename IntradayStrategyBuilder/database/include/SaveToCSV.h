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

namespace fs = std::filesystem;

struct DataPoint {
    std::string equity{""};
    int contractId{0};
    std::string contractType{""};
    int dataType{0};
    std::string data{""};
};

class CSVFileSaver {
    public:
        void start();
        void stop();

        void createDirectoriesAndFiles(const std::string& equity, int contractStrike, const std::string& contractType);
        // For data type, 1 will be ticks, 2 will be five sec data, 3 will be one min data
        void writeDataToFiles(const std::string& equity, int contractId, const std::string& contractType, 
            int dataType, const std::string& data);

        void checkDataQueues();
        void addTicksToQueue(const std::string& equity, int contractId, const std::string& contractType, const std::string& data);
        void addFiveSecDataToQueue(const std::string& equity, int contractId, const std::string& contractType, const std::string& data);
        void addOneMinDataToQueue(const std::string& equity, int contractId, const std::string& contractType, const std::string& data);

    private:
        std::thread dataQueueThread;
        std::queue<DataPoint> dataQueue;

        bool endQueue = false;
};

#endif