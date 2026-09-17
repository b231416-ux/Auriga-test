#include <cassert>
#include <chrono>
#include <iostream>

#include "RateConfig.h"

using namespace std::chrono;

void testOneHour() {

    RateConfig rates(
        50,
        30,
        300
    );

    auto entry =
        system_clock::time_point{};

    auto exit =
        entry + minutes(60);

    double fee =
        rates.calculateFee(entry, exit);

    assert(fee == 50);
}

void testPartialHour() {

    RateConfig rates(
        50,
        30,
        300
    );

    auto entry =
        system_clock::time_point{};

    // 61 minutes should become 2 hours.
    auto exit =
        entry + minutes(61);

    double fee =
        rates.calculateFee(entry, exit);

    assert(fee == 80);
}

void testMultipleHours() {

    RateConfig rates(
        50,
        30,
        300
    );

    auto entry =
        system_clock::time_point{};

    auto exit =
        entry + hours(5);

    double fee =
        rates.calculateFee(entry, exit);

    // 50 + (4 * 30) = 170
    assert(fee == 170);
}

void testDailyCap() {

    RateConfig rates(
        50,
        30,
        300
    );

    auto entry =
        system_clock::time_point{};

    auto exit =
        entry + hours(20);

    double fee =
        rates.calculateFee(entry, exit);

    assert(fee == 300);
}

int main() {

    testOneHour();

    testPartialHour();

    testMultipleHours();

    testDailyCap();

    std::cout
        << "All RateConfig tests passed!\n";

    return 0;
}