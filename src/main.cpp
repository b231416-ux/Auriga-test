#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "ParkingGarage.h"

std::string getBody(const std::string& request) {

    std::size_t position =
        request.find("\r\n\r\n");

    if (position == std::string::npos) {
        return "";
    }

    return request.substr(position + 4);
}


std::string handleClock(
    ParkingGarage& garage,
    const std::string& body
) {
    // Expected:
    // {"timestamp":1789689600}

    std::size_t position =
        body.find("\"timestamp\"");

    if (position == std::string::npos) {
        return R"({"error":"timestamp is required"})";
    }

    position =
        body.find(":", position);

    if (position == std::string::npos) {
        return R"({"error":"invalid timestamp"})";
    }

    position++;

    // Skip spaces and quotation marks.
    while (
        position < body.size() &&
        (
            body[position] == ' ' ||
            body[position] == '"'
        )
    ) {
        position++;
    }

    std::size_t end = position;

    while (
        end < body.size() &&
        body[end] >= '0' &&
        body[end] <= '9'
    ) {
        end++;
    }

    if (end == position) {
        return R"({"error":"timestamp must be Unix seconds"})";
    }

    try {

        long long timestamp =
            std::stoll(
                body.substr(
                    position,
                    end - position
                )
            );

        auto newTime =
            std::chrono::system_clock::time_point(
                std::chrono::seconds(timestamp)
            );

        std::vector<ClosedSession> closedSessions =
            garage.advanceClock(newTime);

        std::ostringstream json;

        json << "{\"closed_sessions\":[";

        for (
            std::size_t i = 0;
            i < closedSessions.size();
            i++
        ) {

            if (i > 0) {
                json << ",";
            }

            json
                << "{"
                << "\"plate\":\""
                << closedSessions[i].plateNumber
                << "\","
                << "\"ticket_id\":\""
                << closedSessions[i].ticketId
                << "\","
                << "\"fee\":"
                << closedSessions[i].fee
                << "}";
        }

        json << "]}";

        return json.str();

    }
    catch (...) {

        return R"({"error":"invalid timestamp"})";
    }
}


int main() {

    // --------------------------------
    // Create parking spots
    // --------------------------------

    std::vector<ParkingSpot> spots = {

        ParkingSpot(1, SpotType::COMPACT),
        ParkingSpot(2, SpotType::COMPACT),

        ParkingSpot(3, SpotType::STANDARD),
        ParkingSpot(4, SpotType::STANDARD),

        ParkingSpot(5, SpotType::EV)
    };


    // --------------------------------
    // Configure rates
    // --------------------------------

    RateConfig rates(
        50.0,
        30.0,
        300.0
    );


    // --------------------------------
    // Create garage
    // --------------------------------

    ParkingGarage garage(
        spots,
        rates
    );


    // --------------------------------
    // Create HTTP server
    // --------------------------------

    int serverSocket =
        socket(
            AF_INET,
            SOCK_STREAM,
            0
        );

    if (serverSocket < 0) {

        std::cerr
            << "Failed to create socket\n";

        return 1;
    }


    int option = 1;

    setsockopt(
        serverSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &option,
        sizeof(option)
    );


    sockaddr_in serverAddress{};

    serverAddress.sin_family =
        AF_INET;

    serverAddress.sin_addr.s_addr =
        INADDR_ANY;

    serverAddress.sin_port =
        htons(8080);


    if (
        bind(
            serverSocket,
            reinterpret_cast<sockaddr*>(
                &serverAddress
            ),
            sizeof(serverAddress)
        ) < 0
    ) {

        std::cerr
            << "Failed to bind port 8080\n";

        close(serverSocket);

        return 1;
    }


    if (
        listen(
            serverSocket,
            10
        ) < 0
    ) {

        std::cerr
            << "Failed to listen\n";

        close(serverSocket);

        return 1;
    }


    std::cout
        << "Parking Garage server running on port 8080\n";


    // --------------------------------
    // Handle HTTP requests
    // --------------------------------

    while (true) {

        sockaddr_in clientAddress{};

        socklen_t clientLength =
            sizeof(clientAddress);


        int clientSocket =
            accept(
                serverSocket,
                reinterpret_cast<sockaddr*>(
                    &clientAddress
                ),
                &clientLength
            );


        if (clientSocket < 0) {
            continue;
        }


        char buffer[8192]{};


        ssize_t bytesRead =
            read(
                clientSocket,
                buffer,
                sizeof(buffer) - 1
            );


        if (bytesRead <= 0) {

            close(clientSocket);

            continue;
        }


        std::string request(
            buffer,
            static_cast<std::size_t>(
                bytesRead
            )
        );


        std::string responseBody;


        // --------------------------------
        // POST /clock
        // --------------------------------

        if (
            request.rfind(
                "POST /clock",
                0
            ) == 0
        ) {

            responseBody =
                handleClock(
                    garage,
                    getBody(request)
                );

        }
        else {

            responseBody =
                R"({"error":"not found"})";
        }


        // --------------------------------
        // HTTP response
        // --------------------------------

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: "
            + std::to_string(
                responseBody.size()
            )
            + "\r\n"
            "Connection: close\r\n"
            "\r\n"
            + responseBody;


        send(
            clientSocket,
            response.c_str(),
            response.size(),
            0
        );


        close(clientSocket);
    }


    close(serverSocket);

    return 0;
}











































