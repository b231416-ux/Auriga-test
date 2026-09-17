#ifndef RATE_CONFIG_H
#define RATE_CONFIG_H

#include <chrono>
#include <string>
#include <unordered_map>

#include "ParkingSpot.h"

struct SpotRates {
    double firstHourRate;
    double additionalHourRate;
    double dailyCap;
};

class RateConfig {
private:
    std::unordered_map<SpotType, SpotRates> rates;

public:
    RateConfig(
        double firstHourRate,
        double additionalHourRate,
        double dailyCap
    );

    RateConfig(
        const std::unordered_map<SpotType, SpotRates>& rates
    );

    static RateConfig fromMessyRateCard(
        const std::string& messyData
    );

    double calculateFee(
        std::chrono::system_clock::time_point entryTime,
        std::chrono::system_clock::time_point exitTime,
        SpotType spotType = SpotType::STANDARD
    ) const;

    SpotRates getRates(SpotType spotType) const;
};

#endif