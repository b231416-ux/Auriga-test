#include "ParkingSpot.h"
#include <stdexcept>

ParkingSpot::ParkingSpot(int id, SpotType type)
    : id(id),
      type(type),
      occupied(false) {
}

int ParkingSpot::getId() const {
    return id;
}

SpotType ParkingSpot::getType() const {
    return type;
}

bool ParkingSpot::isFree() const {
    return !occupied;
}

bool ParkingSpot::canFit(VehicleType vehicleType) const {

    // EV vehicles can ONLY use EV spots.
    if (vehicleType == VehicleType::EV) {
        return type == SpotType::EV;
    }

    // Standard vehicles use standard spots.
    if (vehicleType == VehicleType::STANDARD) {
        return type == SpotType::STANDARD;
    }

    // Compact vehicles use compact spots.
    return type == SpotType::COMPACT;
}

void ParkingSpot::park(
    const std::string& plateNumber
) {

    if (occupied) {
        throw std::runtime_error(
            "Parking spot is already occupied"
        );
    }

    occupied = true;
    vehiclePlate = plateNumber;
}

void ParkingSpot::removeVehicle() {

    occupied = false;
    vehiclePlate.clear();
}

void ParkingSpot::changeVehiclePlate(
    const std::string& newPlate
) {
    if (!occupied) {
        throw std::runtime_error(
            "Cannot change plate of an empty spot"
        );
    }

    vehiclePlate = newPlate;
}