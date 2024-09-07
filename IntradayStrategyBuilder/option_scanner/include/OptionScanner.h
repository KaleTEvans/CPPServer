#ifndef OPTIONSCANNER_H
#define OPTIONSCANNER_H

#include "SaveToCSV.h"
#include "UnderlyingData.h"
#include "ContractData.h"

enum class ScannerNotifications {
    Momentum,
    HighVolume,
    SaveDataOnly
};

class OptionScanner {
    public:
        OptionScanner(std::shared_ptr<tWrapper> wrapper);

        void start();
        void stop();

        // Add UnderlyingData items to be tracked
        void addSecurity(Contract contract, Contract optionBase, ScannerNotifications scn = ScannerNotifications::SaveDataOnly);
        // Remove items
        //void removeSecurity(Contract Contract);

    private:
        // Create option contract requests
        void addOption(Contract option, ScannerNotifications scn, double strikeIncrement);
        // Update option strikes to add new items within chain limit as underlying price changes
        void updateOptionStrikes(std::pair<std::shared_ptr<UnderlyingData>, ScannerNotifications> underlying);
        // Loop that will check for option chain updates every 5 seconds
        void monitorOptionChains();

        std::thread optionScannerThread;
        bool quitOptionScanner{false};

        std::mutex mtx;

        std::shared_ptr<tWrapper> wrapper;
        std::map<std::string, Contract> optionBaseContracts;
        std::vector<std::pair<std::shared_ptr<UnderlyingData>, ScannerNotifications>> trackedTickers;
        std::map<int, std::shared_ptr<ContractData>> trackedCalls;
        std::map<int, std::shared_ptr<ContractData>> trackedPuts;
        std::shared_ptr<CSVFileSaver> csv;
};

#endif