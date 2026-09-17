#include "ParkingGarage.h"

#include <stdexcept>
#include <utility>

ParkingGarage::ParkingGarage(
    std::vector<ParkingSpot> spots,
    const RateConfig& rates
)
    : spots(std::move(spots)),
      rates(rates),
      nextTicketId(1),
      currentTime(std::chrono::system_clock::now()) {
}

ParkingSpot* ParkingGarage::findAvailableSpot(
    VehicleType type
) {
    for (auto& spot : spots) {
        if (spot.isFree() &&
            spot.canFit(type)) {
            return &spot;
        }
    }

    return nullptr;
}

std::string ParkingGarage::checkIn(
    const Vehicle& vehicle
) {
    const std::string& plate =
        vehicle.getPlateNumber();

    if (activeTickets.find(plate)
        != activeTickets.end()) {

        throw std::runtime_error(
            "Vehicle is already parked"
        );
    }

    ParkingSpot* spot =
        findAvailableSpot(
            vehicle.getType()
        );

    if (spot == nullptr) {
        throw std::runtime_error(
            "No suitable parking spot available"
        );
    }

    std::string ticketId =
        "T" + std::to_string(nextTicketId++);

    spot->park(plate);

    // Store the spot type in the ticket.
    ParkingTicket ticket(
        ticketId,
        plate,
        spot->getId(),
        spot->getType(),
        currentTime
    );

    activeTickets.emplace(
        plate,
        std::move(ticket)
    );

    return ticketId;
}

double ParkingGarage::checkOut(
    const std::string& plateNumber
) {
    return checkOutAt(
        plateNumber,
        currentTime
    );
}

double ParkingGarage::checkOutAt(
    const std::string& plateNumber,
    std::chrono::system_clock::time_point exitTime
) {
    auto it =
        activeTickets.find(plateNumber);

    if (it == activeTickets.end()) {
        throw std::runtime_error(
            "Vehicle is not currently parked"
        );
    }

    ParkingTicket& ticket =
        it->second;

    // Use the rate for the actual spot type.
    double fee =
        rates.calculateFee(
            ticket.getEntryTime(),
            exitTime,
            ticket.getSpotType()
        );

    int spotId =
        ticket.getSpotId();

    for (auto& spot : spots) {
        if (spot.getId() == spotId) {
            spot.removeVehicle();
            break;
        }
    }

    activeTickets.erase(it);

    return fee;
}

const ParkingTicket*
ParkingGarage::findVehicle(
    const std::string& plateNumber
) const {
    auto it =
        activeTickets.find(plateNumber);

    if (it == activeTickets.end()) {
        return nullptr;
    }

    return &it->second;
}

bool ParkingGarage::isEVSpotAvailable() const {
    for (const auto& spot : spots) {
        if (spot.getType() == SpotType::EV &&
            spot.isFree()) {
            return true;
        }
    }

    return false;
}

int ParkingGarage::availableSpots(
    SpotType type
) const {
    int count = 0;

    for (const auto& spot : spots) {
        if (spot.getType() == type &&
            spot.isFree()) {
            count++;
        }
    }

    return count;
}

std::vector<ClosedSession>
ParkingGarage::advanceClock(
    std::chrono::system_clock::time_point newTime
) {
    if (newTime < currentTime) {
        throw std::invalid_argument(
            "Garage clock cannot move backwards"
        );
    }

    currentTime = newTime;

    std::vector<ClosedSession> closedSessions;

    std::vector<std::string> platesToClose;

    // Find all sessions parked for more than 24 hours.
    for (const auto& entry : activeTickets) {

        const std::string& plate =
            entry.first;

        const ParkingTicket& ticket =
            entry.second;

        auto parkedDuration =
            currentTime - ticket.getEntryTime();

        if (parkedDuration >
            std::chrono::hours(24)) {

            platesToClose.push_back(plate);
        }
    }

    // Close and bill the sessions.
    for (const std::string& plate :
         platesToClose) {

        auto it =
            activeTickets.find(plate);

        if (it == activeTickets.end()) {
            continue;
        }

        ParkingTicket& ticket =
            it->second;

        double fee =
            rates.calculateFee(
                ticket.getEntryTime(),
                currentTime,
                ticket.getSpotType()
            );

        ClosedSession session{
            plate,
            ticket.getTicketId(),
            fee
        };

        closedSessions.push_back(session);

        int spotId =
            ticket.getSpotId();

        for (auto& spot : spots) {
            if (spot.getId() == spotId) {
                spot.removeVehicle();
                break;
            }
        }

        activeTickets.erase(it);
    }

    return closedSessions;
}

void ParkingGarage::transferSession(
    const std::string& oldPlate,
    const std::string& newPlate
) {
    if (oldPlate.empty() ||
        newPlate.empty()) {

        throw std::invalid_argument(
            "Plate number cannot be empty"
        );
    }

    if (oldPlate == newPlate) {
        throw std::invalid_argument(
            "New plate must be different"
        );
    }

    auto oldIt =
        activeTickets.find(oldPlate);

    if (oldIt == activeTickets.end()) {
        throw std::runtime_error(
            "Vehicle is not currently parked"
        );
    }

    if (activeTickets.find(newPlate)
        != activeTickets.end()) {

        throw std::runtime_error(
            "New plate is already parked"
        );
    }

    ParkingTicket& ticket =
        oldIt->second;

    int spotId =
        ticket.getSpotId();

    // Keep the same ticket, spot and entry time.
    ticket.changePlate(newPlate);

    for (auto& spot : spots) {
        if (spot.getId() == spotId) {
            spot.changeVehiclePlate(newPlate);
            break;
        }
    }

    // Change the map key from old plate to new plate.
    auto node =
        activeTickets.extract(oldIt);

    node.key() = newPlate;

    activeTickets.insert(
        std::move(node)
    );
}

std::chrono::system_clock::time_point
ParkingGarage::getCurrentTime() const {
    return currentTime;
}