#include "Candle.h"

#include <chrono>
#include <iomanip>
#include <sstream>

// Constructor for historical data
Candle::Candle(int reqId, const std::string& date, double open, double high, double low, double close, 
    Decimal volume, int barCount, Decimal WAP)
    : reqId_(reqId), date_(date), time_(0), open_(open), close_(close), high_(high), 
    low_(low), volume_(volume), barCount_(barCount), WAP_(WAP), count_(0)
{
    convertDateToUnix();
    dateConverted_ = true;
}

// Constructor for 5 Second data
Candle::Candle(int reqId, long time, double open, double high, double low,
    double close, Decimal volume, Decimal wap, int count)
    : reqId_(reqId), date_(""), time_(time), open_(open), close_(close), high_(high), 
    low_(low), volume_(volume), barCount_(0), WAP_(wap), count_(count) {}

// Constructor for other candles created from 5 sec
Candle::Candle(int reqId, long time, double open, double high,
    double low, double close, Decimal volume)
    : reqId_(reqId), date_(""), time_(time), open_(open), close_(close), high_(high), 
    low_(low), volume_(volume), barCount_(0), WAP_(0.0), count_(0) {}

Candle::Candle() : reqId_(0), date_(""), time_(0), open_(0), close_(0), high_(0),
    low_(0), volume_(0), barCount_(0), WAP_(0.0), count_(0) {}

Candle::Candle(const Candle& c) : reqId_(c.reqId_), date_(c.date_), dateConverted_(c.dateConverted_),
time_(c.time_), open_(c.open_), close_(c.close_), high_(c.high_), low_(c.low_), volume_(c.volume_),
barCount_(c.barCount_), WAP_(c.WAP_), count_(c.count_) {}

// Getters
int Candle::reqId() const { return reqId_; }
long Candle::time() const { return time_; }
double Candle::open() const { return open_; }
double Candle::close() const { return close_; }
double Candle::high() const { return high_; }
double Candle::low() const { return low_; }
Decimal Candle::volume() const { return volume_; }
int Candle::barCount() const { return barCount_; }
Decimal Candle::WAP() const { return WAP_; }
int Candle::count() const { return count_; }

std::string Candle::date() const {
    if (!dateConverted_) {
        convertUnixToDate();
        dateConverted_ = true;
    }
    return date_;
}

void Candle::convertDateToUnix() {
    // Convert time string to unix values
    std::tm tmStruct = {};
    std::istringstream ss(date_);
    ss >> std::get_time(&tmStruct, "%Y%m%d %H:%M:%S");

    time_t stime = std::mktime(&tmStruct);
    time_ = static_cast<long>(stime);
}

void Candle::convertUnixToDate() const {
    struct tm* timeInfo;
    char buffer[80];

    time_t time = static_cast<time_t>(time_);
    timeInfo = localtime(&time);

    strftime(buffer, sizeof(buffer), "%Y%m%d %H:%M:%S", timeInfo);
    std::string date = buffer;
    date_ = date;
}

void Candle::printCandle() const {
    std::cout << "ID: " << reqId_ << " | Date: " << this->date() << " | Time: " << time_ << " Close: " << close_ <<
        " | Volume: " << decimalStringToDisplay(volume_).c_str()<< " | Traded Count: " << count_ << std::endl;
}