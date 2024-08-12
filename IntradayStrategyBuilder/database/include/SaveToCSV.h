#ifndef SAVETOCSV_H
#define SAVETOCSV_H

#include <iostream>
#include <filesystem>  
#include <fstream>    
#include <ctime>      
#include <string> 
#include <vector>
#include <memory>

#include "ContractData.h"

namespace fs = std::filesystem;

void createDirectoriesAndFiles(const std::string& equity, int contractStrike, const std::string& contractType) {
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

    std::cout << "Directory and files created at: " << fullPath << std::endl;
}

// For data type, 1 will be ticks, 2 will be five sec data, 3 will be one min data
void writeDataToFiles(const std::string& equity, int contractId, const std::string& contractType, 
    int dataType, const std::string& data) {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    std::string parentDirName = std::to_string(1900 + localTime->tm_year) + "_" +
                                std::to_string(1 + localTime->tm_mon) + "_" + equity;
    std::string dirName = std::to_string(localTime->tm_mday) + "_" + equity;

    fs::path fullPath = fs::path(parentDirName) / dirName;
    std::string dataFile;

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

#endif