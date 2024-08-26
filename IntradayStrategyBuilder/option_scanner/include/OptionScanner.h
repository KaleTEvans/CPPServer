#ifndef OPTIONSCANNER_H
#define OPTIONSCANNER_H

#include "SaveToCSV.h"
#include "ContractData.h"

class OptionScanner {
    public:
        OptionScanner(std::shared_ptr<tWrapper> wrapper);

        void start();
        void stop();
        void addSecurity(const std::string& ticker);

    private:
        std::vector<int> getOptionsChain();

        std::shared_ptr<tWrapper> wrapper;
        std::vector<std::string> trackedTickers;
};

#endif