#include "OptionScanner.h"

OptionScanner::OptionScanner(std::shared_ptr<tWrapper> wrapper) : wrapper(wrapper) {
    // Create CSV instance
    csv = std::make_shared<CSVFileSaver>();
}

void OptionScanner::start() {
    for (const auto& underlying : trackedTickers) {
        // Send requests for individual underlying contracts
        underlying.first->startReceivingData();
    }

    // Start csv collection
    csv->start();

    // Now start the thread to run monitorOptionChains
    optionScannerThread = std::thread([this]() {
        monitorOptionChains();
    });
}

void OptionScanner::stop() {
    quitOptionScanner = true;
    if (optionScannerThread.joinable()) optionScannerThread.join();
    // Cancel all open requests
    for (const auto& underlying : trackedTickers) underlying.first->stopReceivingData();
    for (const auto& call : trackedCalls) call.second->cancelDataStream();
    for (const auto& put : trackedPuts) put.second->cancelDataStream();
    // Clear containers to drop items out of scope and delete
    trackedTickers.clear();
    trackedCalls.clear();
    trackedPuts.clear();
    // End csv collection
    csv->stop();
}

void OptionScanner::addSecurity(Contract contract, Contract optionBase, ScannerNotifications scn) {
    std::shared_ptr<UnderlyingData> underlying = std::make_shared<UnderlyingData>(wrapper, csv, contract);

    trackedTickers.push_back({underlying, scn});

    // Add option contract to list
    optionBaseContracts.insert({optionBase.symbol, optionBase});
}

void OptionScanner::addOption(Contract option, ScannerNotifications scn, double strikeIncrement) {
    int mktDataId = wrapper->reqMktData(option, "100,101,106,104,225,232,233,293,294,295,411", false, false);
    int rtbId = wrapper->reqRealTimeBars(option, 5, "TRADES", true);

    std::shared_ptr<ContractData> cd = std::make_shared<ContractData>(wrapper, csv, mktDataId, rtbId, option, strikeIncrement);
    
    if (option.right == "C") trackedCalls.insert({option.strike, cd});
    else trackedPuts.insert({option.strike, cd});
}

void OptionScanner::updateOptionStrikes(std::pair<std::shared_ptr<UnderlyingData>, ScannerNotifications> underlying) {
    // Get up to date optioms chain based on price
    std::pair<std::vector<double>, std::vector<double>> strikes = underlying.first->getStrikes();
    int strikeIncrement = underlying.first->getStrikeIncrement();

    // Retrieve the correct base option contract to edit
    Contract optionBase;
    std::string ticker = underlying.first->getContract().symbol;
    auto it = optionBaseContracts.find(ticker);
    if (it == optionBaseContracts.end()) {
        std::cout << "Contract not found in option scanner" << std::endl;
        return;
    } else {
        optionBase = optionBaseContracts[ticker];
    }

    // Loop through strikes to create option contracts and begin tracking if not currently tracked
    for (const auto& callStrike : strikes.first) {
        if (trackedCalls.find(callStrike) == trackedCalls.end()) {
            optionBase.strike = callStrike;
            optionBase.right = "C";
            addOption(optionBase, underlying.second, strikeIncrement);
        }
    }
    for (const auto& putStrike : strikes.second) {
        if (trackedPuts.find(putStrike) == trackedPuts.end()) {
            optionBase.strike = putStrike;
            optionBase.right = "P";
            addOption(optionBase, underlying.second, strikeIncrement);
        }
    }
}

void OptionScanner::monitorOptionChains() {
    while (!quitOptionScanner) {
        for (const auto& underlying : trackedTickers) {
            updateOptionStrikes(underlying);
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}