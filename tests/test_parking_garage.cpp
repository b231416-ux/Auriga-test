#include <cassert>
#include <iostream>
#include <vector>

#include "ParkingGarage.h"

// Test normal vehicle check-in and lookup.
void testVehicleCheckInAndLookup() {

    std::vector<ParkingSpot> spots = {
        ParkingSpot(1, SpotType::COMPACT),
        ParkingSpot(2, SpotType::STANDARD),
        ParkingSpot(3, SpotType::EV)
    };

    RateConfig rates(50, 30, 300);

    ParkingGarage garage(spots, rates);

    Vehicle car(
        "RJ14AB1234",
        VehicleType::STANDARD
    );

    std::string ticketId =
        garage.checkIn(car);

    assert(ticketId == "T1");

    const ParkingTicket* ticket =
        garage.findVehicle("RJ14AB1234");

    assert(ticket != nullptr);
    assert(ticket->getPlateNumber() == "RJ14AB1234");
    assert(ticket->getSpotId() == 2);
}


// Test that EV vehicles get EV spots.
void testEVVehicleGetsEVSpot() {

    std::vector<ParkingSpot> spots = {
        ParkingSpot(1, SpotType::STANDARD),
        ParkingSpot(2, SpotType::EV)
    };

    RateConfig rates(50, 30, 300);

    ParkingGarage garage(spots, rates);

    Vehicle ev(
        "RJ14EV5678",
        VehicleType::EV
    );

    garage.checkIn(ev);

    const ParkingTicket* ticket =
        garage.findVehicle("RJ14EV5678");

    assert(ticket != nullptr);

    // EV vehicle must use EV spot.
    assert(ticket->getSpotId() == 2);
}


// Test EV spot availability.
void testEVAvailability() {

    std::vector<ParkingSpot> spots = {
        ParkingSpot(1, SpotType::EV)
    };

    RateConfig rates(50, 30, 300);

    ParkingGarage garage(spots, rates);

    assert(garage.isEVSpotAvailable());

    Vehicle ev(
        "RJ14EV1111",
        VehicleType::EV
    );

    garage.checkIn(ev);

    assert(!garage.isEVSpotAvailable());
}


// Test that the same vehicle cannot check in twice.
void testDuplicateVehicleCheckIn() {

    std::vector<ParkingSpot> spots = {
        ParkingSpot(1, SpotType::STANDARD),
        ParkingSpot(2, SpotType::STANDARD)
    };

    RateConfig rates(50, 30, 300);

    ParkingGarage garage(spots, rates);

    Vehicle car(
        "RJ14AB1234",
        VehicleType::STANDARD
    );

    garage.checkIn(car);

    bool exceptionThrown = false;

    try {
        garage.checkIn(car);
    }
    catch (const std::exception&) {
        exceptionThrown = true;
    }

    assert(exceptionThrown);
}


// Test when there is no suitable spot.
void testNoSuitableSpot() {

    std::vector<ParkingSpot> spots = {
        ParkingSpot(1, SpotType::STANDARD)
    };

    RateConfig rates(50, 30, 300);

    ParkingGarage garage(spots, rates);

    Vehicle ev(
        "RJ14EV9999",
        VehicleType::EV
    );

    bool exceptionThrown = false;

    try {
        garage.checkIn(ev);
    }
    catch (const std::exception&) {
        exceptionThrown = true;
    }

    assert(exceptionThrown);
}


// Test checkout and spot release.
void testCheckoutRemovesVehicle() {

    std::vector<ParkingSpot> spots = {
        ParkingSpot(1, SpotType::STANDARD)
    };

    RateConfig rates(50, 30, 300);

    ParkingGarage garage(spots, rates);

    Vehicle car(
        "RJ14XY1234",
        VehicleType::STANDARD
    );

    garage.checkIn(car);

    assert(
        garage.findVehicle("RJ14XY1234")
        != nullptr
    );

    garage.checkOut("RJ14XY1234");

    // Vehicle should no longer be active.
    assert(
        garage.findVehicle("RJ14XY1234")
        == nullptr
    );

    // Spot should be free again.
    assert(
        garage.availableSpots(
            SpotType::STANDARD
        ) == 1
    );
}


// Test checkout of a vehicle that does not exist.
void testUnknownVehicleCheckout() {

    std::vector<ParkingSpot> spots = {
        ParkingSpot(1, SpotType::STANDARD)
    };

    RateConfig rates(50, 30, 300);

    ParkingGarage garage(spots, rates);

    bool exceptionThrown = false;

    try {
        garage.checkOut("UNKNOWN");
    }
    catch (const std::exception&) {
        exceptionThrown = true;
    }

    assert(exceptionThrown);
}


// T6: Test valet transfer to a different plate.
void testTransferSession() {

    std::vector<ParkingSpot> spots = {
        ParkingSpot(1, SpotType::STANDARD)
    };

    RateConfig rates(50, 30, 300);

    ParkingGarage garage(spots, rates);

    Vehicle car(
        "RJ14OLD123",
        VehicleType::STANDARD
    );

    // Check the original vehicle in.
    garage.checkIn(car);

    const ParkingTicket* beforeTransfer =
        garage.findVehicle("RJ14OLD123");

    assert(beforeTransfer != nullptr);

    // Save the original session information.
    int originalSpotId =
        beforeTransfer->getSpotId();

    auto originalEntryTime =
        beforeTransfer->getEntryTime();

    std::string originalTicketId =
        beforeTransfer->getTicketId();

    // Transfer the session to a new plate.
    garage.transferSession(
        "RJ14OLD123",
        "RJ14NEW456"
    );

    // Old plate must no longer be active.
    assert(
        garage.findVehicle("RJ14OLD123")
        == nullptr
    );

    // New plate must now be active.
    const ParkingTicket* afterTransfer =
        garage.findVehicle("RJ14NEW456");

    assert(afterTransfer != nullptr);

    // Spot must remain the same.
    assert(
        afterTransfer->getSpotId()
        == originalSpotId
    );

    // Entry time must remain the same.
    assert(
        afterTransfer->getEntryTime()
        == originalEntryTime
    );

    // Ticket must remain the same.
    assert(
        afterTransfer->getTicketId()
        == originalTicketId
    );

    // The new plate should be stored.
    assert(
        afterTransfer->getPlateNumber()
        == "RJ14NEW456"
    );

    // The same spot must still be occupied,
    // so no standard spot should be available.
    assert(
        garage.availableSpots(
            SpotType::STANDARD
        ) == 0
    );
}


int main() {

    testVehicleCheckInAndLookup();

    testEVVehicleGetsEVSpot();

    testEVAvailability();

    testDuplicateVehicleCheckIn();

    testNoSuitableSpot();

    testCheckoutRemovesVehicle();

    testUnknownVehicleCheckout();

    // T6
    testTransferSession();

    std::cout
        << "All ParkingGarage tests passed!\n";

    return 0;
}