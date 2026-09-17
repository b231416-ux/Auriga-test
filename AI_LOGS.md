
A busy multi-level city-centre parking garage. Cars come and go all day, and the attendant needs to check a car in, check it out, and charge the right fee. Rates are tiered — the first hour is one price, each extra hour is cheaper, and there’s a daily cap so nobody is overcharged for a long stay; part-hours round up. Spots are limited and come in types — compact, standard, and EV (with a charger) — and an EV must get an EV spot. Drivers keep asking ‘is an EV spot free right now?’ and the attendant hunts for a car by its plate. By evening the log is huge.
Build the attendant something so every car is charged correctly and no spot is double-parked.
(The attendant’s day is the spec — build it for any garage, not one. Get check-in / check-out and the fee right first, then the spot types and lookups.)

This is a good Low-Level Design (LLD / OOP) problem for a Parking Garage System.

The important part of the requirement is:

Check-in a vehicle and assign a free spot.
Check-out and calculate the fee correctly.
Fee rules:
First hour → first-hour rate.
Every additional started hour → cheaper rate.
Daily maximum cap.
Part-hours are rounded up.
Spot types:
Compact
Standard
EV
EV vehicles must use EV spots.
Check whether an EV spot is currently available.
Find a parked vehicle by license plate.
Never assign the same spot to two vehicles.
The design should work for any garage, not hardcoded for one particular garage.
C++ LLD
#include <bits/stdc++.h>
using namespace std;

// ---------------- VEHICLE ----------------

enum class VehicleType {
    COMPACT,
    STANDARD,
    EV
};

class Vehicle {
private:
    string plateNumber;
    VehicleType type;

public:
    Vehicle(string plateNumber, VehicleType type)
        : plateNumber(plateNumber), type(type) {}

    string getPlateNumber() const {
        return plateNumber;
    }

    VehicleType getType() const {
        return type;
    }
};


// ---------------- PARKING SPOT ----------------

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
    string parkedVehicle;

public:
    ParkingSpot(int id, SpotType type)
        : id(id), type(type), occupied(false) {}

    int getId() const {
        return id;
    }

    SpotType getType() const {
        return type;
    }

    bool isFree() const {
        return !occupied;
    }

    bool canFit(VehicleType vehicleType) const {
        // EV vehicle MUST use EV spot
        if (vehicleType == VehicleType::EV)
            return type == SpotType::EV;

        if (vehicleType == VehicleType::STANDARD)
            return type == SpotType::STANDARD;

        return type == SpotType::COMPACT;
    }

    void park(const string& plate) {
        if (occupied)
            throw runtime_error("Spot is already occupied");

        occupied = true;
        parkedVehicle = plate;
    }

    void removeVehicle() {
        occupied = false;
        parkedVehicle = "";
    }
};


// ---------------- TICKET ----------------

class ParkingTicket {
private:
    string ticketId;
    string plateNumber;
    int spotId;

    chrono::system_clock::time_point entryTime;

public:
    ParkingTicket(string ticketId,
                  string plateNumber,
                  int spotId)
        : ticketId(ticketId),
          plateNumber(plateNumber),
          spotId(spotId),
          entryTime(chrono::system_clock::now()) {}

    string getTicketId() const {
        return ticketId;
    }

    string getPlateNumber() const {
        return plateNumber;
    }

    int getSpotId() const {
        return spotId;
    }

    chrono::system_clock::time_point getEntryTime() const {
        return entryTime;
    }
};


// ---------------- RATE CONFIGURATION ----------------

class RateConfig {
private:
    double firstHourRate;
    double additionalHourRate;
    double dailyCap;

public:
    RateConfig(double firstHourRate,
               double additionalHourRate,
               double dailyCap)
        : firstHourRate(firstHourRate),
          additionalHourRate(additionalHourRate),
          dailyCap(dailyCap) {}

    double calculateFee(
        chrono::system_clock::time_point entryTime,
        chrono::system_clock::time_point exitTime) const {

        auto duration =
            chrono::duration_cast<chrono::minutes>(
                exitTime - entryTime
            ).count();

        // Part-hours round UP.
        long long hours = (duration + 59) / 60;

        // Minimum one hour.
        hours = max(1LL, hours);

        double fee;

        if (hours == 1) {
            fee = firstHourRate;
        }
        else {
            fee = firstHourRate +
                  (hours - 1) * additionalHourRate;
        }

        // Daily cap.
        fee = min(fee, dailyCap);

        return fee;
    }
};


// ---------------- GARAGE ----------------

class ParkingGarage {
private:
    vector<ParkingSpot> spots;

    // plate -> ticket
    unordered_map<string, ParkingTicket> activeTickets;

    RateConfig rates;

    int nextTicketId = 1;

public:
    ParkingGarage(vector<ParkingSpot> spots,
                  RateConfig rates)
        : spots(move(spots)), rates(rates) {}

    // Find suitable free spot.
    ParkingSpot* findAvailableSpot(VehicleType type) {

        for (auto& spot : spots) {

            if (spot.isFree() &&
                spot.canFit(type)) {

                return &spot;
            }
        }

        return nullptr;
    }

    // ---------------- CHECK IN ----------------

    string checkIn(const Vehicle& vehicle) {

        // Vehicle already parked?
        if (activeTickets.count(vehicle.getPlateNumber())) {
            throw runtime_error(
                "Vehicle is already parked"
            );
        }

        ParkingSpot* spot =
            findAvailableSpot(vehicle.getType());

        if (spot == nullptr) {
            throw runtime_error(
                "No suitable parking spot available"
            );
        }

        string ticketId =
            "T" + to_string(nextTicketId++);

        spot->park(vehicle.getPlateNumber());

        ParkingTicket ticket(
            ticketId,
            vehicle.getPlateNumber(),
            spot->getId()
        );

        activeTickets.emplace(
            vehicle.getPlateNumber(),
            ticket
        );

        return ticketId;
    }


    // ---------------- CHECK OUT ----------------

    double checkOut(const string& plateNumber) {

        auto it = activeTickets.find(plateNumber);

        if (it == activeTickets.end()) {
            throw runtime_error(
                "Vehicle is not currently parked"
            );
        }

        ParkingTicket& ticket = it->second;

        auto exitTime =
            chrono::system_clock::now();

        double fee =
            rates.calculateFee(
                ticket.getEntryTime(),
                exitTime
            );

        // Free the spot.
        for (auto& spot : spots) {

            if (spot.getId() == ticket.getSpotId()) {
                spot.removeVehicle();
                break;
            }
        }

        // Remove active parking record.
        activeTickets.erase(it);

        return fee;
    }


    // ---------------- FIND VEHICLE ----------------

    ParkingTicket* findVehicle(
        const string& plateNumber) {

        auto it = activeTickets.find(plateNumber);

        if (it == activeTickets.end())
            return nullptr;

        return &it->second;
    }


    // ---------------- EV AVAILABILITY ----------------

    bool isEVSpotAvailable() const {

        for (const auto& spot : spots) {

            if (spot.getType() == SpotType::EV &&
                spot.isFree()) {

                return true;
            }
        }

        return false;
    }


    // ---------------- AVAILABLE SPOTS ----------------

    int availableSpots(SpotType type) const {

        int count = 0;

        for (const auto& spot : spots) {

            if (spot.getType() == type &&
                spot.isFree()) {

                count++;
            }
        }

        return count;
    }
};
Example usage
int main() {

    vector<ParkingSpot> spots = {

        ParkingSpot(1, SpotType::COMPACT),
        ParkingSpot(2, SpotType::COMPACT),

        ParkingSpot(3, SpotType::STANDARD),
        ParkingSpot(4, SpotType::STANDARD),

        ParkingSpot(5, SpotType::EV)
    };

    // First hour = ₹50
    // Additional hour = ₹30
    // Daily cap = ₹300

    RateConfig rates(
        50,
        30,
        300
    );

    ParkingGarage garage(
        spots,
        rates
    );


    Vehicle car(
        "RJ14AB1234",
        VehicleType::STANDARD
    );

    Vehicle ev(
        "RJ14EV5678",
        VehicleType::EV
    );


    // Check in
    string ticket1 =
        garage.checkIn(car);

    cout << "Car ticket: "
         << ticket1 << endl;


    // EV availability
    cout << "EV spot available: "
         << (garage.isEVSpotAvailable()
             ? "Yes"
             : "No")
         << endl;


    // Find vehicle
    ParkingTicket* ticket =
        garage.findVehicle("RJ14AB1234");

    if (ticket) {
        cout << "Vehicle found" << endl;
        cout << "Spot ID: "
             << ticket->getSpotId()
             << endl;
    }


    // Check out
    double fee =
        garage.checkOut("RJ14AB1234");

    cout << "Parking fee: ₹"
         << fee << endl;


    return 0;
}
Why this design satisfies the requirements
Requirement	Design
Check-in	checkIn()
Check-out	checkOut()
First-hour pricing	RateConfig
Cheaper additional hours	additionalHourRate
Daily cap	dailyCap
Part-hour rounding	(minutes + 59) / 60
Compact spots	SpotType::COMPACT
Standard spots	SpotType::STANDARD
EV spots	SpotType::EV
EV → EV only	canFit()
Check EV availability	isEVSpotAvailable()
Find vehicle by plate	unordered_map
Prevent double parking	occupied flag + controlled park()
Fast vehicle lookup	unordered_map → average O(1)
Any garage	Spots/rates are passed into ParkingGarage
One important improvement for a real system

The fee calculation above applies the daily cap to the total stay. For example, with:

First hour = ₹50
Additional hour = ₹30
Daily cap = ₹300

A 1-hour stay → ₹50
A 2-hour stay → ₹80
A 5-hour stay → ₹170
A 10-hour stay → ₹300 (cap reached)

For a production system, I would also make the rate policy pluggable so different garages can have different pricing rules without changing ParkingGarage. This is where the Strategy Pattern fits naturally.

I completed till here now tell me step by step what to do
I got this update
now what to do in 5c
these are the results now we have only 20 mins to complete this so please do it fas t
all commands are done but in web i got this