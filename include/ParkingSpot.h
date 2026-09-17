#ifndef PARKING_SPOT_H
#define PARKING_SPOT_H

#include <string>
#include "Vehicle.h"

enum class SpotType {
    COMPACT,
    STANDARD,
    EV
};

class ParkingSpot {
private:
    int id;
    SpotType type;
    bool occupied;
    std::string vehiclePlate;

public:
    ParkingSpot(int id, SpotType type);

    int getId() const;

    SpotType getType() const;

    bool isFree() const;

    bool canFit(VehicleType vehicleType) const;

    void park(const std::string& plateNumber);

    void removeVehicle();
    void changeVehiclePlate(
    const std::string& newPlate
);
};

#endif