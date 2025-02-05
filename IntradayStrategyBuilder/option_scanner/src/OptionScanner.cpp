#include "OptionScanner.h"

OptionScanner::OptionScanner(std::shared_ptr<tWrapper> wrapper, 
    std::shared_ptr<ScannerNotificationBus> notifications,
    std::shared_ptr<SocketDataCollector> sdc) : 
    wrapper(wrapper), notifications(notifications), sdc(sdc) {
    // Create CSV instance
    csv = std::make_shared<CSVFileSaver>();

    // Subscribe to news events
    wrapper->getMessageBus()->subscribe(EventType::TickNewsInfo, [this](std::shared_ptr<DataEvent> event) {
            this->handleTickNewsEvent(std::dynamic_pointer_cast<TickNewsEvent>(event));
        });
}

void OptionScanner::start() {
    for (const auto& underlying : trackedTickers) {
        // Send requests for individual underlying contracts
        underlying->startReceivingData();
    }

    // Now start the thread to run monitorOptionChains
    optionScannerThread = std::thread([this]() {
        monitorOptionChains();
    });

    optScannerRunning = true;
    std::cout << "Option Scanner Started" << std::endl;
}

void OptionScanner::stop() {
    quitOptionScanner = true;
    if (optionScannerThread.joinable()) optionScannerThread.join();
    // Cancel all open requests
    for (const auto& call : trackedCalls) call.second->cancelDataStream();
    for (const auto& put : trackedPuts) put.second->cancelDataStream();
    for (const auto& underlying : trackedTickers) underlying->stopReceivingData();
    // Clear containers to drop items out of scope and delete
    trackedCalls.clear();
    trackedPuts.clear();
    trackedTickers.clear();
    optionBaseContracts.clear();
}

bool OptionScanner::checkScannerRunning() { return optScannerRunning; }

void OptionScanner::addSecurity(Contract contract, Contract optionBase) {
    std::shared_ptr<UnderlyingData> underlying = std::make_shared<UnderlyingData>(wrapper, sdc, contract);

    trackedTickers.push_back(underlying);

    // Add option contract to list
    optionBaseContracts.insert({optionBase.symbol, optionBase});
}

void OptionScanner::addOption(Contract option, double strikeIncrement, double underlyingPrice) {
    int mktDataId = wrapper->reqMktData(option, "100,101,106,104,225,232,233,293,294,295,411", false, false);
    int rtbId = wrapper->reqRealTimeBars(option, 5, "TRADES", true);

    std::shared_ptr<ContractData> cd = std::make_shared<ContractData>(wrapper, sdc, notifications, mktDataId, 
        rtbId, option, strikeIncrement, underlyingPrice);
    
    if (option.right == "C") trackedCalls.insert({option.strike, cd});
    else trackedPuts.insert({option.strike, cd});
}

void OptionScanner::updateOptionStrikes(std::shared_ptr<UnderlyingData> underlying) {
    // Get up to date optioms chain based on price
    std::pair<std::vector<double>, std::vector<double>> strikes = underlying->getStrikes();
    int strikeIncrement = underlying->getStrikeIncrement();

    // Retrieve the correct base option contract to edit
    Contract optionBase;
    std::string ticker = underlying->getContract().symbol;
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
            addOption(optionBase, strikeIncrement, underlying->getLastPrice());
        }
    }
    for (const auto& putStrike : strikes.second) {
        if (trackedPuts.find(putStrike) == trackedPuts.end()) {
            optionBase.strike = putStrike;
            optionBase.right = "P";
            addOption(optionBase, strikeIncrement, underlying->getLastPrice());
        }
    }
}

void OptionScanner::monitorOptionChains() {
    while (!quitOptionScanner) {
        std::unique_lock<std::mutex> lock(mtx);
        for (const auto& underlying : trackedTickers) {
            updateOptionStrikes(underlying);

            // Loop over tracked Contract data objects and update underlying price
            for (auto& i : trackedCalls) { i.second->updateUnderlyingPrice(underlying->getLastPrice()); }
            for (auto& i : trackedPuts) { i.second->updateUnderlyingPrice(underlying->getLastPrice()); }
        }
        lock.unlock();

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void OptionScanner::handleTickNewsEvent(std::shared_ptr<TickNewsEvent> event) {
    long timeStamp = event->dateTime;

    ContractNewsData cnd;
    cnd.time = timeStamp;
    cnd.articleId = event->articleId;
    cnd.headline = event->headline;
    cnd.sentimentScore = event->sentimentScore;
    if (event->hasFullExtraData) sdc->sendNewsData(cnd.serializeNewsObject());

    event->print();
}