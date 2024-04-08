#pragma once
#ifndef CANDLE_H
#define CANDLE_H

#include <iostream>

class Candle {
public:
    // Constructor for historical data
    Candle(int reqId, const std::string& date, double open, double high, double low, double close
        , int volume, int barCount, double WAP);

    // Constructor for 5 Second data
    Candle(int reqId, long time, double open, double high, double low, double close, long volume, double wap, int count);

    // Constructor for other candles created from 5 sec
    Candle(int reqId, long time, double open, double high, double low, double close, long volume);

    Candle();

    Candle(const Candle& c); // Copy constructor

    int reqId() const;
    std::string date() const;
    long time() const;
    double open() const;
    double close() const;
    double high() const;
    double low() const;
    long volume() const;
    int barCount() const;
    double WAP() const;
    int hasGaps() const;
    int count() const;

    void convertDateToUnix();
    void convertUnixToDate() const; // Lazy conversion only upon request
    
private:
    int reqId_;
    mutable std::string date_;
    mutable bool dateConverted_{ false }; // Marked false if a unix time is received in the constructor
    long time_;
    double open_;
    double close_;
    double high_;
    double low_;
    long volume_;
    int barCount_;
    double WAP_;
    int count_;
};

#endif