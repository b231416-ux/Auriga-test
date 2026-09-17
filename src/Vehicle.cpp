#include "Vehicle.h"

Vehicle::Vehicle(
    const std::string& plateNumber,
    VehicleType type
)
    : plateNumber(plateNumber),
      type(type) {
}

const std::string& Vehicle::getPlateNumber() const {
    return plateNumber;
}

VehicleType Vehicle::getType() const {
    return type;
}