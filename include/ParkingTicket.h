#ifndef PARKING_TICKET_H
#define PARKING_TICKET_H

#include <chrono>
#include <string>

#include "ParkingSpot.h"

class ParkingTicket {
private:
    std::string ticketId;
    std::string plateNumber;
    int spotId;
    SpotType spotType;

    std::chrono::system_clock::time_point entryTime;

public:
    // Backward-compatible constructor
    ParkingTicket(
        const std::string& ticketId,
        const std::string& plateNumber,
        int spotId
    );

    // Constructor using the current time
    ParkingTicket(
        const std::string& ticketId,
        const std::string& plateNumber,
        int spotId,
        SpotType spotType
    );

    // Constructor with a specific entry time
    ParkingTicket(
        const std::string& ticketId,
        const std::string& plateNumber,
        int spotId,
        SpotType spotType,
        std::chrono::system_clock::time_point entryTime
    );

    const std::string& getTicketId() const;

    const std::string& getPlateNumber() const;

    int getSpotId() const;

    SpotType getSpotType() const;

    std::chrono::system_clock::time_point getEntryTime() const;

    void changePlate(const std::string& newPlate);
};

#endif