#include "ParkingTicket.h"

ParkingTicket::ParkingTicket(
    const std::string& ticketId,
    const std::string& plateNumber,
    int spotId
)
    : ParkingTicket(
        ticketId,
        plateNumber,
        spotId,
        SpotType::STANDARD
    ) {
}

ParkingTicket::ParkingTicket(
    const std::string& ticketId,
    const std::string& plateNumber,
    int spotId,
    SpotType spotType
)
    : ticketId(ticketId),
      plateNumber(plateNumber),
      spotId(spotId),
      spotType(spotType),
      entryTime(std::chrono::system_clock::now()) {
}

ParkingTicket::ParkingTicket(
    const std::string& ticketId,
    const std::string& plateNumber,
    int spotId,
    SpotType spotType,
    std::chrono::system_clock::time_point entryTime
)
    : ticketId(ticketId),
      plateNumber(plateNumber),
      spotId(spotId),
      spotType(spotType),
      entryTime(entryTime) {
}

const std::string& ParkingTicket::getTicketId() const {
    return ticketId;
}

const std::string& ParkingTicket::getPlateNumber() const {
    return plateNumber;
}

int ParkingTicket::getSpotId() const {
    return spotId;
}

SpotType ParkingTicket::getSpotType() const {
    return spotType;
}

std::chrono::system_clock::time_point
ParkingTicket::getEntryTime() const {
    return entryTime;
}

void ParkingTicket::changePlate(
    const std::string& newPlate
) {
    plateNumber = newPlate;
}