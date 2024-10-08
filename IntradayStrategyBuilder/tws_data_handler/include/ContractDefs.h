//===================================================
// Contains a list of predefined contracts
//===================================================

#include <iostream>

#pragma once
#ifndef CONTRACTDEFS_H
#define CONTRACTDEFS_H

struct Contract;

class ContractDefs {
public:
    static Contract SPXInd();
    static Contract BZBroadTape();
    static Contract SPXOpt(const std::string& expDate, const std::string& right, int strike);
    static Contract SPXOpt0DTE(const std::string& right, int strike);
    static Contract SPY();
    static Contract QQQ();
    static Contract VIX();
    static Contract AAPL();
    static int getConId(const std::string& symbol);

    static Contract emptyContract();
};

#endif