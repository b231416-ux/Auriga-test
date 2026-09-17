cmake_minimum_required(VERSION 3.16)

project(AurigaParkingGarage)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/Vehicle.cpp
    src/ParkingSpot.cpp
    src/ParkingTicket.cpp
    src/RateConfig.cpp
    src/ParkingGarage.cpp
)

add_executable(
    parking_garage
    src/main.cpp
    ${SOURCES}
)

add_executable(
    test_vehicle
    tests/test_vehicle.cpp
    src/Vehicle.cpp
)

add_executable(
    test_parking_spot
    tests/test_parking_spot.cpp
    src/Vehicle.cpp
    src/ParkingSpot.cpp
)

add_executable(
    test_parking_ticket
    tests/test_parking_ticket.cpp
    src/ParkingTicket.cpp
)

add_executable(
    test_rate_config
    tests/test_rate_config.cpp
    src/RateConfig.cpp
)

add_executable(
    test_parking_garage
    tests/test_parking_garage.cpp
    ${SOURCES}
)