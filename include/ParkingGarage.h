#ifndef PARKING_GARAGE_H
#define PARKING_GARAGE_H

#include <chrono>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ParkingSpot.h"
#include "ParkingTicket.h"
#include "RateConfig.h"
#include "Vehicle.h"

struct ClosedSession {
    std::string plateNumber;
    std::string ticketId;
    double fee;
};

class ParkingGarage {
private:
    std::vector<ParkingSpot> spots;

    // Maps license plate -> active parking ticket.
    std::unordered_map<std::string, ParkingTicket> activeTickets;

    RateConfig rates;

    int nextTicketId;

    // Current garage clock.
    std::chrono::system_clock::time_point currentTime;

    ParkingSpot* findAvailableSpot(VehicleType type);

    double checkOutAt(
        const std::string& plateNumber,
        std::chrono::system_clock::time_point exitTime
    );

public:
    ParkingGarage(
        std::vector<ParkingSpot> spots,
        const RateConfig& rates
    );

    // Check a vehicle into the garage.
    std::string checkIn(
        const Vehicle& vehicle
    );

    // Check a vehicle out using the current garage time.
    double checkOut(
        const std::string& plateNumber
    );

    // Look up an active vehicle by license plate.
    const ParkingTicket* findVehicle(
        const std::string& plateNumber
    ) const;

    // Check whether at least one EV spot is free.
    bool isEVSpotAvailable() const;

    // Count available spots of a particular type.
    int availableSpots(
        SpotType type
    ) const;

    // T2:
    // Move the garage clock forward.
    // Automatically closes sessions parked for more than 24 hours.
    std::vector<ClosedSession> advanceClock(
        std::chrono::system_clock::time_point newTime
    );

    // T6:
    // Transfer an active parking session to a new plate.
    // The spot and entry time remain unchanged.
    void transferSession(
        const std::string& oldPlate,
        const std::string& newPlate
    );

    // Return the current garage time.
    std::chrono::system_clock::time_point getCurrentTime() const;
};

#endif