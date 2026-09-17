#include <iostream>
#include "Vehicle.h"

int main() {

    Vehicle car(
        "RJ14AB1234",
        VehicleType::STANDARD
    );

    std::cout << "Vehicle created successfully!\n";

    std::cout
        << "Plate Number: "
        << car.getPlateNumber()
        << "\n";

    return 0;
}