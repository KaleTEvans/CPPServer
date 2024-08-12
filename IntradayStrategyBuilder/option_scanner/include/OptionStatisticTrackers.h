#ifndef OPTIONSTATISTICTRACKERS_H
#define OPTIONSTATISTICTRACKERS_H

#include <algorithm>
#include <cmath>

// Since it would be too costly to save and store every single tick, several statistical tracking methods
// will be aggregated in real time. 


class StandardDeviation {
private:
    int n;

    double sum_{ 0 };
    double sumSq_{ 0 };
    double mean_{ 0 };
    double variance_{ 0 };
    double stdDev_{ 0 };

public:
    StandardDeviation() : n(0) {}

    void addValue(double x) {
        n++;
        sum_ += x;
        sumSq_ += x * x;
        mean_ = sum_ / n;
        variance_ = (sumSq_ - n * mean_ * mean_) / n;
        stdDev_ = std::sqrt(variance_);
    }

    double sum() const { return n; }
    double stDev() const { return stdDev_; }
    double mean() const { return mean_; }
    double variance() const { return variance_; }
};

#endif