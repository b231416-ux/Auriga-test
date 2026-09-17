Auriga Parking Garage

A C++17 parking garage management system developed for the Auriga coding assessment.

The system supports vehicle check-in/check-out, parking spot allocation, tiered parking fees, EV parking, vehicle lookup, messy rate-card processing, automatic closure of sessions parked for more than 24 hours, and valet session transfer.

Features
Multi-spot parking garage
Compact, Standard, and EV parking spots
Vehicle check-in
Vehicle check-out
Prevents double parking
EV vehicles can only use EV spots
Vehicle lookup by license plate
Parking spot availability lookup
Tiered parking rates
Partial hours are rounded up
Daily parking fee cap
Different rates for different parking spot types
Messy rate-card import and cleaning
Automatic closure and billing of sessions parked for more than 24 hours
Valet hand-off/session transfer to another license plate
HTTP POST /clock endpoint
C++ unit tests
CMake build system
Technologies Used
C++17
Standard Template Library (STL)
CMake
POSIX sockets
Git/GitHub
Project Structure

Auriga-test/

├── README.md
├── REASONING.md
├── AI_LOGS.md
├── CMakeLists.txt
├── .gitignore
│
├── include/
│ ├── ParkingGarage.h
│ ├── ParkingSpot.h
│ ├── ParkingTicket.h
│ ├── RateConfig.h
│ └── Vehicle.h
│
├── src/
│ ├── main.cpp
│ ├── ParkingGarage.cpp
│ ├── ParkingSpot.cpp
│ ├── ParkingTicket.cpp
│ ├── RateConfig.cpp
│ └── Vehicle.cpp
│
└── tests/
├── test_parking_garage.cpp
├── test_parking_spot.cpp
├── test_parking_ticket.cpp
├── test_rate_config.cpp
└── test_vehicle.cpp

Requirements

The project requires:

C++ compiler supporting C++17
CMake 3.16 or newer
Linux, WSL, or GitHub Codespaces

The HTTP server uses POSIX socket APIs.

Build Instructions

Open a terminal in the repository root:

cd /workspaces/Auriga-test

Create the build directory:

mkdir -p build

Enter the build directory:

cd build

Configure the project:

cmake ..

Build the project:

cmake --build .

After a successful build, the following executables are created:

parking_garage
test_vehicle
test_parking_spot
test_parking_ticket
test_rate_config
test_parking_garage
Running the Application

From the build directory:

./parking_garage

The HTTP server starts on port 8080.

Expected output:

Parking Garage server running on port 8080

The server can then be accessed through:

http://localhost:8080
API Endpoints
POST /clock

Advances the internal garage clock.

This endpoint is used for the T2 automation requirement.

Request:

POST /clock
Content-Type: application/json

Request body:

{
  "timestamp": 1789689600
}

The timestamp is a Unix timestamp in seconds.

Example:

curl -X POST http://localhost:8080/clock -H "Content-Type: application/json" -d '{"timestamp":1789689600}'

Response when no sessions need to be automatically closed:

{
  "closed_sessions": []
}

If a session has been parked for more than 24 hours:

{
  "closed_sessions": [
    {
      "plate": "RJ14AB1234",
      "ticket_id": "T1",
      "fee": 300
    }
  ]
}

The automatically closed session is billed, removed from active sessions, and its parking spot is released.

Core Parking Operations

The main parking functionality is implemented by the ParkingGarage class.

Important operations include:

checkIn()
checkOut()
findVehicle()
availableSpots()
isEVSpotAvailable()
transferSession()
advanceClock()
Parking Check-In

When a vehicle checks in:

The license plate is checked.
The system prevents an already parked vehicle from checking in again.
A suitable free parking spot is searched.
The parking spot is occupied.
A parking ticket is generated.
The ticket records the entry time.
The active session is stored using the vehicle's license plate.
Parking Check-Out

When a vehicle checks out:

The active session is located using the license plate.
Parking duration is calculated.
The correct rate for the parking spot type is selected.
Partial hours are rounded up.
The daily cap is applied.
The parking spot is released.
The active session is removed.
The calculated parking fee is returned.
Parking Rates

The system supports:

First-hour rate
Additional-hour rate
Daily cap

Rates can be configured independently for:

COMPACT
STANDARD
EV

For example, with:

First hour:       50
Additional hour:  30
Daily cap:        300

the charges are:

1 hour  -> 50
2 hours -> 80
3 hours -> 110

Partial hours are rounded up.

For example:

1 hour 10 minutes -> 2 billed hours

The final fee cannot exceed the configured daily cap.

T4 — Messy Rate Card

The assessment includes a messy rate-card requirement.

The project provides:

RateConfig::fromMessyRateCard()

This function processes rate-card text and extracts the usable rates for:

COMPACT
STANDARD
EV

The extracted values are stored in a cleaned internal rate configuration.

Fee calculation then uses the cleaned rates.

T2 — Automatic Closure After 24 Hours

The assessment requires an automated process that closes and bills sessions parked for more than 24 hours.

The garage maintains an internal clock:

currentTime

The clock can be advanced using:

advanceClock(newTime)

When the clock advances, active sessions are checked.

A session is automatically closed when:

current time - entry time > 24 hours

The automatic closure:

Calculates the parking fee.
Closes the session.
Releases the parking spot.
Removes the session from active parking.
Returns the closed session information.

The HTTP endpoint for this requirement is:

POST /clock
T6 — Valet Session Transfer

The valet hand-off requirement allows an active parking session to be transferred from one license plate to another.

Example:

Old plate: RJ14OLD123
New plate: RJ14NEW456

The following information remains unchanged:

Ticket ID
Parking spot
Parking spot type
Original entry time

Only the active license plate is changed.

The system prevents:

Empty plate numbers
Transferring to the same plate
Transferring a non-existent session
Transferring to a plate that is already parked
EV Parking

EV vehicles require EV parking spots.

The system provides:

isEVSpotAvailable()

to determine whether an EV spot is currently available.

A vehicle cannot be assigned to an incompatible parking spot.

Vehicle Lookup

Active vehicles can be found using their license plate.

The system provides:

findVehicle()

If the vehicle is currently parked, its active parking ticket is returned.

If the vehicle is not parked, no active ticket is returned.

Testing

The project contains separate tests for the major components.

Build the project first:

cd build
cmake --build .

Run the tests individually:

./test_vehicle

./test_parking_spot

./test_parking_ticket

./test_rate_config

./test_parking_garage

Run all tests together:

./test_vehicle && ./test_parking_spot && ./test_parking_ticket && ./test_rate_config && ./test_parking_garage

Successful tests print confirmation messages such as:

All Vehicle tests passed!
All ParkingSpot tests passed!
All ParkingTicket tests passed!
All RateConfig tests passed!
All ParkingGarage tests passed!
Debugging
Clean Rebuild

If the project has build issues:

cd /workspaces/Auriga-test/build
cmake --build . --clean-first

To reconfigure the project:

cd /workspaces/Auriga-test/build
cmake ..
Check Git Status

From the repository root:

cd /workspaces/Auriga-test
git status
Test the HTTP Endpoint

Start the server:

./parking_garage

Open another terminal and run:

curl -X POST http://localhost:8080/clock -H "Content-Type: application/json" -d '{"timestamp":1789689600}'

Expected response:

{
  "closed_sessions": []
}
Browser Shows "Not Found"

Opening the forwarded URL directly in a browser sends a GET request.

The implemented assessment endpoint is:

POST /clock

Therefore, seeing:

{"error":"not found"}

when opening the root URL directly is expected.

Use curl with POST /clock to test the endpoint.

Assessment Requirements Implemented
T4 — Messy Data

Implemented per-spot-type rate configuration and messy rate-card parsing.

T2 — Automation

Implemented the internal garage clock and:

POST /clock

for automatic closure and billing of sessions parked for more than 24 hours.

T6 — Lifecycle

Implemented valet session transfer while preserving:

Parking spot
Entry time
Ticket ID
Spot type
Verification

The project was built successfully using CMake.

The test executables were executed successfully.

The POST /clock endpoint was also tested using curl.

The repository is maintained using Git and the final implementation is pushed to the main branch.

License

This project was developed as part of the Auriga coding assessment.