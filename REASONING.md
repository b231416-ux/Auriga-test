Reasoning Behind the Solution
1. Problem Understanding

The task is to build a parking garage management system that can handle vehicles entering and leaving a garage while correctly managing parking spots and parking fees.

The updated assessment adds three additional requirements:

T4 — Messy rate-card data
T2 — Automatic closure of sessions parked for more than 24 hours
T6 — Transfer of an active parking session to another license plate

The solution was designed so that the core parking logic is independent from the HTTP interface and can be tested separately.

2. System Design

The project is divided into five main classes.

Vehicle

The Vehicle class stores:

License plate number
Vehicle type

Supported vehicle types are:

COMPACT
STANDARD
EV
ParkingSpot

The ParkingSpot class represents a physical parking space.

It stores:

Spot ID
Spot type
Occupied/free state
Vehicle license plate

It is responsible for checking whether a vehicle can use the spot and for parking/removing a vehicle.

ParkingTicket

The ParkingTicket class represents an active parking session.

It stores:

Ticket ID
License plate
Spot ID
Spot type
Entry time

The entry time is important because it is used to calculate the parking fee and to determine whether a session has exceeded 24 hours.

RateConfig

The RateConfig class is responsible for parking prices.

It supports separate rates for:

COMPACT
STANDARD
EV

Each spot type has:

First-hour rate
Additional-hour rate
Daily cap

It also contains the messy rate-card parsing functionality required by T4.

ParkingGarage

The ParkingGarage class coordinates the complete parking process.

It manages:

Parking spots
Active parking tickets
Ticket generation
Check-in
Check-out
Vehicle lookup
Spot availability
Automatic closure
Session transfer
3. Data Structures

Active parking sessions are stored using an unordered map:

license plate -> ParkingTicket

This provides efficient lookup by license plate.

Parking spots are stored in a vector because the garage needs to iterate through available spots when assigning a vehicle.

The combination of a vector for physical spots and an unordered map for active sessions keeps the implementation simple while providing efficient vehicle lookup.

4. Check-In Logic

When a vehicle checks in, the following process is used:

Check whether the license plate already has an active parking session.
If the vehicle is already parked, reject the request.
Search for a free compatible parking spot.
If no suitable spot exists, reject the check-in.
Generate a new ticket ID.
Mark the selected spot as occupied.
Create a parking ticket.
Store the ticket in the active-session map.
Return the ticket ID.

This ensures that the same vehicle cannot be parked twice and that a vehicle is never assigned to an incompatible spot.

5. Parking Spot Compatibility

The garage supports three spot types:

COMPACT
STANDARD
EV

An EV vehicle must use an EV spot.

The canFit() operation is used to determine whether a parking spot is compatible with a vehicle.

The system also provides an EV availability lookup through:

isEVSpotAvailable()
6. Check-Out and Fee Calculation

When a vehicle checks out:

Locate its active ticket using the license plate.
Obtain the entry time from the ticket.
Calculate the elapsed parking time.
Round partial hours upward.
Apply the first-hour rate.
Apply the additional-hour rate for subsequent hours.
Apply the daily cap.
Release the parking spot.
Remove the active ticket.
Return the calculated fee.

For example, if the rates are:

First hour = 50
Additional hour = 30
Daily cap = 300

then:

1 hour  = 50
2 hours = 80
3 hours = 110

A partial hour is rounded upward.

For example:

1 hour 10 minutes = 2 billed hours
7. T4 — Messy Rate Card

The updated assessment requires the system to process a messy rate card.

The implementation provides:

RateConfig::fromMessyRateCard()

The function processes the supplied rate-card text and extracts the usable numeric rates associated with:

COMPACT
STANDARD
EV

The extracted values are converted into a clean internal configuration.

The pricing calculation operates on this cleaned configuration rather than directly using the raw input.

This separates data cleaning from the actual parking-fee calculation.

8. T2 — Automatic Closure

The assessment requires a nightly job that automatically closes and bills sessions parked for more than 24 hours.

To make this behavior deterministic and testable, the garage maintains its own internal clock.

The clock is stored as:

currentTime

The clock can be advanced using:

advanceClock(newTime)

When the clock is advanced, all active sessions are examined.

If:

currentTime - entryTime > 24 hours

the session is automatically closed.

The automatic closure performs the following operations:

Calculate the parking fee.
Create a closed-session record.
Release the parking spot.
Remove the active parking ticket.
Return the closed-session information.

The HTTP endpoint:

POST /clock

is provided to expose this clock functionality.

The endpoint accepts a Unix timestamp.

Example request:

{
  "timestamp": 1789689600
}
9. Why an Internal Clock Was Used

Using the actual system clock directly would make automatic-closure testing difficult because tests would have to wait for real time to pass.

Instead, the garage maintains an internal clock.

This allows tests to simulate:

One hour later
One day later
More than 24 hours later

without actually waiting.

It also makes the T2 behavior deterministic.

10. T6 — Valet Session Transfer

The T6 requirement allows an open parking session to be transferred from one license plate to another.

For example:

Old plate: RJ14OLD123
New plate: RJ14NEW456

The transfer keeps the same:

Ticket ID
Parking spot
Spot type
Original entry time

Only the license plate changes.

Both the active ticket and the physical parking spot are updated.

The active-session map is also changed from:

old plate -> ticket

to:

new plate -> same ticket

The transfer operation rejects invalid situations such as:

Empty license plates
Same old and new plate
Old plate not currently parked
New plate already being parked
11. Preventing Double Parking

Before checking in a vehicle, the garage checks the active-ticket map.

If the license plate already exists, the check-in operation throws an error.

This ensures that one license plate cannot have two active parking sessions.

The same protection is applied during session transfer so that a session cannot be transferred to a plate that is already parked.

12. Error Handling

The implementation uses C++ exceptions for invalid operations.

Examples include:

Vehicle already parked
Vehicle not currently parked
No suitable parking spot
EV spot unavailable
Invalid session transfer
Moving the garage clock backwards
Missing rate configuration
Invalid rate-card data

This prevents invalid state changes and keeps the core logic consistent.

13. Testing Strategy

The project contains separate tests for the main components.

Vehicle Tests

The vehicle tests verify:

License plate
Vehicle type
Parking Spot Tests

The parking spot tests verify:

Spot ID
Spot type
Occupancy
Vehicle compatibility
Parking
Vehicle removal
Parking Ticket Tests

The ticket tests verify:

Ticket ID
License plate
Spot ID
Spot type
Entry time
Rate Configuration Tests

The rate tests verify:

First-hour pricing
Additional-hour pricing
Partial-hour rounding
Daily cap
Rate configuration
Parking Garage Tests

The garage tests verify:

Check-in
Check-out
Duplicate parking prevention
Vehicle lookup
EV availability
Parking spot availability
Session transfer
Preservation of ticket information during transfer
Automatic session closure
14. Issues Found During Development
Issue 1 — Build Directory

The generated build/ directory contains compiled files and should not be committed to the repository.

It was therefore added to .gitignore.

Issue 2 — Spot Type in Parking Ticket

The original ticket model did not retain the parking spot type.

This was changed so that each ticket knows whether it belongs to a:

COMPACT
STANDARD
EV

spot.

This allows the correct rate to be selected during checkout.

Issue 3 — Session Transfer

Updating only the ticket's license plate would leave the physical parking spot containing the old license plate.

Therefore, both the ticket and parking spot are updated during a transfer.

Issue 4 — Automatic Clock Testing

Using only the real system time would make the 24-hour automation difficult to test.

An internal garage clock was introduced so the tests can move time forward deterministically.

Issue 5 — HTTP Compilation Error

While implementing the /clock endpoint, the HTTP response initially attempted to access a field named:

plate

However, the ClosedSession structure stores the license plate as:

plateNumber

This caused a compilation error.

The incorrect field was changed from:

closedSessions[i].plate

to:

closedSessions[i].plateNumber

The project was then rebuilt successfully.

Issue 6 — Browser Request

Opening the forwarded server URL directly in a browser produced:

{"error":"not found"}

This happened because the browser sends a GET request.

The assessment endpoint is:

POST /clock

The endpoint was therefore tested using a POST request with curl.

15. HTTP Testing

The /clock endpoint was tested using:

curl -X POST http://localhost:8080/clock -H "Content-Type: application/json" -d '{"timestamp":1789689600}'

The server returned:

{"closed_sessions":[]}

This confirmed that the HTTP endpoint was reachable and processing the request.

16. Build Verification

The project was configured and built using CMake.

The build command used was:

cmake --build .

The project compiled successfully.

17. Test Verification

The following test executables were executed:

./test_vehicle
./test_parking_spot
./test_parking_ticket
./test_rate_config
./test_parking_garage

All implemented tests passed successfully.

18. Final Git Verification

After implementation and testing, the project was committed and pushed to the main branch.

The final Git status showed that the working tree was clean and the local branch was synchronized with the remote repository.

19. Conclusion

The final solution separates the core parking logic into maintainable C++ classes and provides deterministic handling of pricing, parking sessions, automatic closure, and session transfer.

The updated T4, T2, and T6 requirements are incorporated into the implementation and covered by the testing strategy.

Important: REASONING.md documents the implementation decisions and testing process. It does not replace AI_LOGS.md. The AI_LOGS.md file must contain the complete AI conversation exactly as required by the assessment.


