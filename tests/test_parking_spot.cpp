#include <iostream>
#include "ParkingSpot.h"

int main() {

    ParkingSpot compactSpot(
        1,
        SpotType::COMPACT
    );

    ParkingSpot standardSpot(
        2,
        SpotType::STANDARD
    );

    ParkingSpot evSpot(
        3,
        SpotType::EV
    );


    Vehicle compactCar(
        "RJ14AA1111",
        VehicleType::COMPACT
    );

    Vehicle standardCar(
        "RJ14BB2222",
        VehicleType::STANDARD
    );

    Vehicle evCar(
        "RJ14EV3333",
        VehicleType::EV
    );


    // Test correct spot types
    std::cout
        << "Compact car -> Compact spot: "
        << (compactSpot.canFit(compactCar.getType())
            ? "YES"
            : "NO")
        << "\n";

    std::cout
        << "Standard car -> Standard spot: "
        << (standardSpot.canFit(standardCar.getType())
            ? "YES"
            : "NO")
        << "\n";

    std::cout
        << "EV car -> EV spot: "
        << (evSpot.canFit(evCar.getType())
            ? "YES"
            : "NO")
        << "\n";


    // Test EV restriction
    std::cout
        << "EV car -> Standard spot: "
        << (standardSpot.canFit(evCar.getType())
            ? "YES"
            : "NO")
        << "\n";


    // Test parking
    compactSpot.park(
        compactCar.getPlateNumber()
    );

    std::cout
        << "Compact spot free after parking: "
        << (compactSpot.isFree()
            ? "YES"
            : "NO")
        << "\n";


    // Test removing vehicle
    compactSpot.removeVehicle();

    std::cout
        << "Compact spot free after checkout: "
        << (compactSpot.isFree()
            ? "YES"
            : "NO")
        << "\n";


    return 0;
}