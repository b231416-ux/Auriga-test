#include <iostream>
#include "ParkingTicket.h"

int main() {

    ParkingTicket ticket(
        "T1",
        "RJ14AB1234",
        5
    );

    std::cout << "Ticket created successfully!\n";

    std::cout
        << "Ticket ID: "
        << ticket.getTicketId()
        << "\n";

    std::cout
        << "Plate Number: "
        << ticket.getPlateNumber()
        << "\n";

    std::cout
        << "Spot ID: "
        << ticket.getSpotId()
        << "\n";

    return 0;
}