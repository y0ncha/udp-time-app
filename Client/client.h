/**
 * @file client.h
 * @brief Declares the TimeClient class for UDP time server communication.
 *
 * The TimeClient class provides an interface for sending time-related requests
 * to a UDP time server and receiving responses. It supports various request types
 * and manages socket initialization, cleanup, and user interaction.
 *
 * C++14 is used for compatibility.
 */
#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <string>
#include <vector>
#include <iostream>
#include "utils.h"

/**
 * @class TimeClient
 * @brief UDP client for time server communication.
 */
class TimeClient {
public:
    /**
     * @brief Constructs a TimeClient for the given server IP and port.
     * @param serverIp IP address of the time server.
     * @param port Port number of the time server.
     *
     * Initializes member variables and calls initialize().
     */
    TimeClient(const std::string& serverIp, unsigned short port);

    /**
     * @brief Destructor. Cleans up resources.
     *
     * Closes the socket and cleans up Winsock if initialized.
     */
    ~TimeClient();

    /**
     * @struct Request
     * @brief Represents a structured request to the server.
     */
    struct Request {
        Request();
        Request(ReqCode c) : code(c), args() {};
        Request(ReqCode c, const std::vector<std::string>& a) : code(c), args(a) {};
        ReqCode code;                  // Request code
        std::vector<std::string> args; // Arguments for the request
    };

    /**
     * @brief Sends a request message to the server.
     * @param message The message to send to the server.
     * @return true on success, false on error.
     * @note Calls initialize() if not already done.
     */
    bool sendRequest(const std::vector<char>& message);
    bool sendRequest(ReqCode request);
    bool sendRequest(const TimeClient::Request& request);

    /**
     * @brief Receives a response from the server.
     * @param response Reference to a string/vector/uint32_t where the response will be stored.
     * @return true on success, false on error.
     */
    bool receiveResponse(std::vector<char>& response);
    bool receiveResponse(std::string& response);
    bool receiveResponse(uint32_t& response);

    /**
     * @brief Main client loop. Displays menu and handles user input.
     * @return true if exited normally, false otherwise.
     */
    bool run();

private:
    /**
     * @brief Initializes Winsock and socket.
     * @return true on success, false on error.
     */
    bool initialize();

    /**
     * @brief Cleans up Winsock and socket resources.
     */
    void cleanup();

    /**
     * @brief Dispatches the request code to the appropriate handler.
     * @param code Request code.
     * @return true if successful, false otherwise.
     */
    bool dispatch(ReqCode code);

    /**
     * @brief Encodes a Request object into a vector of bytes for sending.
     * @param request Request object.
     * @return Encoded request as vector<char>.
     */
    std::vector<char> incode(const Request& request);

    /**
     * @brief Checks if the response indicates an error.
     * @param response Response vector.
     * @return true if error, false otherwise.
     */
    bool isError(const std::vector<char>& response);

    // Request handlers for each supported operation
    // 1. Get current date and time
    bool GetTime();
    // 2. Get current time (no date)
    bool GetTimeWithoutDate();
    // 3. Get seconds since epoch
    bool GetTimeSinceEpoch();
    // 4. Estimate client-to-server delay
    bool GetClientToServerDelayEstimation();
    // 5. Measure round-trip time (RTT)
    bool MeasureRTT();
    // 6. Get time without seconds
    bool GetTimeWithoutDateOrSeconds();
    // 7. Get current year
    bool GetYear();
    // 8. Get current month and day
    bool GetMonthAndDay();
    // 9. Get seconds since beginning of month
    bool GetSecondsSinceBeginningOfMonth();
    // 10. Get week number of year
    bool GetWeekOfYear();
    // 11. Get daylight savings status
    bool GetDaylightSavings();
    // 12. Get time in another city
    bool GetTimeWithoutDateInCity();
    // 13. Measure time lap
    bool MeasureTimeLap();

    std::string serverIp_;      // Server IP address
    unsigned short port_;       // Server port
    SOCKET connSocket_;         // UDP socket
    sockaddr_in serverAddr_;    // Server address structure
    bool initialized_;          // Winsock initialization state
    bool debug_ = false;        // Debug mode flag
};