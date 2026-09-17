#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>

enum class VehicleType {
    COMPACT,
    STANDARD,
    EV
};

class Vehicle {
private:
    std::string plateNumber;
    VehicleType type;

public:
    Vehicle(const std::string& plateNumber, VehicleType type);

    const std::string& getPlateNumber() const;

    VehicleType getType() const;
};

#endif