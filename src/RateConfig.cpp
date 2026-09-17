#include "RateConfig.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <stdexcept>
#include <sstream>

namespace {

SpotType parseSpotType(const std::string& text) {
    std::string s;

    for (char c : text) {
        s += static_cast<char>(
            std::toupper(static_cast<unsigned char>(c))
        );
    }

    if (s.find("COMPACT") != std::string::npos) {
        return SpotType::COMPACT;
    }

    if (s.find("EV") != std::string::npos) {
        return SpotType::EV;
    }

    return SpotType::STANDARD;
}

}

RateConfig::RateConfig(
    double firstHourRate,
    double additionalHourRate,
    double dailyCap
) {
    rates[SpotType::COMPACT] =
        {firstHourRate, additionalHourRate, dailyCap};

    rates[SpotType::STANDARD] =
        {firstHourRate, additionalHourRate, dailyCap};

    rates[SpotType::EV] =
        {firstHourRate, additionalHourRate, dailyCap};
}

RateConfig::RateConfig(
    const std::unordered_map<SpotType, SpotRates>& inputRates
)
    : rates(inputRates) {
}

RateConfig RateConfig::fromMessyRateCard(
    const std::string& messyData
) {
    std::unordered_map<SpotType, SpotRates> cleaned;

    std::stringstream stream(messyData);
    std::string line;

    std::regex numberRegex(
        R"([-+]?[0-9]*\.?[0-9]+)"
    );

    while (std::getline(stream, line)) {

        std::smatch typeMatch;

        if (line.find("COMPACT") != std::string::npos ||
            line.find("compact") != std::string::npos) {

            std::vector<double> values;

            auto begin =
                std::sregex_iterator(
                    line.begin(),
                    line.end(),
                    numberRegex
                );

            auto end = std::sregex_iterator();

            for (auto it = begin; it != end; ++it) {
                values.push_back(
                    std::stod(it->str())
                );
            }

            if (values.size() >= 3) {
                cleaned[SpotType::COMPACT] =
                    {values[0], values[1], values[2]};
            }
        }

        else if (line.find("STANDARD") != std::string::npos ||
                 line.find("standard") != std::string::npos) {

            std::vector<double> values;

            auto begin =
                std::sregex_iterator(
                    line.begin(),
                    line.end(),
                    numberRegex
                );

            auto end = std::sregex_iterator();

            for (auto it = begin; it != end; ++it) {
                values.push_back(
                    std::stod(it->str())
                );
            }

            if (values.size() >= 3) {
                cleaned[SpotType::STANDARD] =
                    {values[0], values[1], values[2]};
            }
        }

        else if (line.find("EV") != std::string::npos ||
                 line.find("ev") != std::string::npos) {

            std::vector<double> values;

            auto begin =
                std::sregex_iterator(
                    line.begin(),
                    line.end(),
                    numberRegex
                );

            auto end = std::sregex_iterator();

            for (auto it = begin; it != end; ++it) {
                values.push_back(
                    std::stod(it->str())
                );
            }

            if (values.size() >= 3) {
                cleaned[SpotType::EV] =
                    {values[0], values[1], values[2]};
            }
        }
    }

    if (cleaned.size() != 3) {
        throw std::invalid_argument(
            "Rate card must contain rates for "
            "COMPACT, STANDARD and EV"
        );
    }

    return RateConfig(cleaned);
}

double RateConfig::calculateFee(
    std::chrono::system_clock::time_point entryTime,
    std::chrono::system_clock::time_point exitTime,
    SpotType spotType
) const {

    auto it = rates.find(spotType);

    if (it == rates.end()) {
        throw std::invalid_argument(
            "No rate configured for spot type"
        );
    }

    const SpotRates& rate = it->second;

    auto duration =
        std::chrono::duration_cast<std::chrono::minutes>(
            exitTime - entryTime
        ).count();

    long long hours =
        (duration + 59) / 60;

    hours = std::max(1LL, hours);

    double fee;

    if (hours == 1) {
        fee = rate.firstHourRate;
    } else {
        fee =
            rate.firstHourRate +
            (hours - 1) * rate.additionalHourRate;
    }

    return std::min(fee, rate.dailyCap);
}

SpotRates RateConfig::getRates(
    SpotType spotType
) const {
    auto it = rates.find(spotType);

    if (it == rates.end()) {
        throw std::invalid_argument(
            "No rate configured for spot type"
        );
    }

    return it->second;
}