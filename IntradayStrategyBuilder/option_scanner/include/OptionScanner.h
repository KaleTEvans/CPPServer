#ifndef OPTIONSCANNER_H
#define OPTIONSCANNER_H

#include "SaveToCSV.h"
#include "UnderlyingData.h"
#include "ContractData.h"
#include "ScannerNotificationHandler.h"

class OptionScanner {
    public:
        OptionScanner(std::shared_ptr<tWrapper> wrapper, std::shared_ptr<ScannerNotificationBus> notifications);

        void start();
        void stop();

        // Add UnderlyingData items to be tracked
        void addSecurity(Contract contract, Contract optionBase);
        // Remove items
        //void removeSecurity(Contract Contract);

    private:
        // Create option contract requests
        void addOption(Contract option, double strikeIncrement);
        // Update option strikes to add new items within chain limit as underlying price changes
        void updateOptionStrikes(std::shared_ptr<UnderlyingData> underlying);
        // Loop that will check for option chain updates every 5 seconds
        void monitorOptionChains();

        std::thread optionScannerThread;
        bool quitOptionScanner{false};

        std::mutex mtx;

        std::shared_ptr<tWrapper> wrapper;
        std::shared_ptr<ScannerNotificationBus> notifications;
        std::map<std::string, Contract> optionBaseContracts;
        std::vector<std::shared_ptr<UnderlyingData>> trackedTickers;
        std::map<int, std::shared_ptr<ContractData>> trackedCalls;
        std::map<int, std::shared_ptr<ContractData>> trackedPuts;
        std::shared_ptr<CSVFileSaver> csv;
};

#endif