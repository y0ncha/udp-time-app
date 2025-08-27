/**
 * @file client.cpp
 * @brief Implements the TimeClient class for UDP time requests to a server.
 *
 * This file contains the implementation of the TimeClient class, which communicates
 * with a time server using UDP sockets. It supports various time-related requests
 * and handles user interaction via a menu-driven interface.
 *
 * C++14 is used for compatibility.
 */

#include "client.h"
#include <algorithm>
#include <cctype>

 /**
  * @brief Constructs a TimeClient and initializes Winsock and socket.
  * @param serverIp IP address of the server.
  * @param port Port number to connect to.
  */
TimeClient::TimeClient(const std::string& serverIp, unsigned short port)
    : serverIp_(serverIp), port_(port), connSocket_(INVALID_SOCKET), initialized_(false)
{
    memset(&serverAddr_, 0, sizeof(serverAddr_));
    initialize();
}

/**
 * @brief Destructor. Cleans up resources.
 */
TimeClient::~TimeClient() {
    cleanup();
}

/**
 * @brief Initializes Winsock and UDP socket, sets up server address.
 * @return true if successful, false otherwise.
 */
bool TimeClient::initialize() {
    WSAData wsaData;
    if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        printError("WSAStartup");
        return false;
    }
    initialized_ = true;

    connSocket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (INVALID_SOCKET == connSocket_) {
        printError("socket");
        cleanup();
        return false;
    }

    serverAddr_.sin_family = AF_INET;
    serverAddr_.sin_addr.s_addr = inet_addr(serverIp_.c_str());
    if (serverAddr_.sin_addr.s_addr == INADDR_NONE) {
        printError("inet_addr");
        cleanup();
        return false;
    }
    serverAddr_.sin_port = htons(port_);
    return true;
}

/**
 * @brief Cleans up socket and Winsock resources.
 */
void TimeClient::cleanup() {
    if (connSocket_ != INVALID_SOCKET) {
        closesocket(connSocket_);
        connSocket_ = INVALID_SOCKET;
    }
    if (initialized_) {
        WSACleanup();
        initialized_ = false;
    }
}

/**
 * @brief Sends a UDP request to the server.
 * @param message Message to send.
 * @return true if successful, false otherwise.
 */
bool TimeClient::sendRequest(const std::vector<char>& message) {
    int bytesSent = sendto(connSocket_, message.data(), static_cast<int>(message.size()), 0,
        (const sockaddr*)&serverAddr_, sizeof(serverAddr_));
    if (SOCKET_ERROR == bytesSent) {
        printError("sendto");
        return false;
    }
    if (debug_) std::cout << "Sent: " << bytesSent << "/" << message.size() << " bytes.\n";
    return true;
}

/**
 * @brief Sends a request code to the server.
 * @param request Request code.
 * @return true if successful, false otherwise.
 */
bool TimeClient::sendRequest(ReqCode request) {
    std::vector<char> message(1);
    message[0] = static_cast<char>(request);
    return sendRequest(message);
}

/**
 * @brief Sends a structured request to the server.
 * @param request_ Request object.
 * @return true if successful, false otherwise.
 */
bool TimeClient::sendRequest(const TimeClient::Request& request_) {
    std::vector<char> request = incode(request_);
    return sendRequest(request);
}

/**
 * @brief Receives a UDP response from the server as a vector of bytes.
 * @param response Vector to store response.
 * @return true if successful, false otherwise.
 */
bool TimeClient::receiveResponse(std::vector<char>& response) {
    std::vector<char> recvBuff(BUFFER_SIZE);
    int bytesRecv = recv(connSocket_, recvBuff.data(), static_cast<int>(recvBuff.size()), 0);
    if (SOCKET_ERROR == bytesRecv || isError(recvBuff)) {
        printError("receiveResponse");
        return false;
    }
    recvBuff.resize(bytesRecv);
    response.assign(recvBuff.begin(), recvBuff.end());
    if (debug_) std::cout << "Received: " << bytesRecv << " bytes.\n";
    return true;
}

/**
 * @brief Receives a string response from the server.
 * @param response String to store response.
 * @return true if successful, false otherwise.
 */
bool TimeClient::receiveResponse(std::string& response) {
    std::vector<char> recvBuff(BUFFER_SIZE);
    if (!receiveResponse(recvBuff)) {
        return false;
    }
    response.assign(recvBuff.begin(), recvBuff.end());
    return true;
}

/**
 * @brief Receives a uint32_t response from the server.
 * @param response Variable to store response.
 * @return true if successful, false otherwise.
 */
bool TimeClient::receiveResponse(uint32_t& response) {
    std::vector<char> recvBuff(BUFFER_SIZE);
    if (!receiveResponse(recvBuff)) {
        return false;
    }
    if (recvBuff.size() > sizeof(uint32_t)) {
        std::cout << "Invalid response size for uint32_t.\n";
        return false;
    }
    response = toUint32(recvBuff);
    return true;
}

/**
 * @brief Main client loop. Displays menu and handles user input.
 * @return true if exited normally, false otherwise.
 */
bool TimeClient::run() {
    if (!initialized_) {
        std::cout << "TimeClient: Not initialized properly.\n";
        return false;
    }

    while (true) {
        system("cls");
        printMenu();
        std::cout << "Enter your choice (1-13) or 0 to exit: ";
        std::string input;
        std::cin >> input;

        // Only allow up to two digits, and must be numeric
        if (input.empty() || input.size() > 2 || !std::all_of(input.begin(), input.end(), ::isdigit)) {
            system("cls");
            printMenu();
            std::cout << "Invalid choice. Please enter a number between 0 and 13 (max two digits).\n";
            system("pause");
            continue;
        }

        int choice = std::stoi(input);
        if (choice == 0) {
            system("cls");
            printMenu();
            std::cout << "Time Client: Closing Connection.\n";
            break;
        }
        if (choice < 1 || choice > 13) {
            system("cls");
            printMenu();
            std::cout << "Invalid choice. Please select a valid option (1-13) or 0 to exit.\n";
            system("pause");
            continue;
        }
        ReqCode code = static_cast<ReqCode>(choice);
        system("cls");
        printMenu();
        if (!dispatch(code)) {
            std::cout << "Request failed.\n";
        }
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }
    return true;
}

/**
 * @brief Dispatches the request code to the appropriate handler.
 * @param code Request code.
 * @return true if successful, false otherwise.
 */
bool TimeClient::dispatch(ReqCode code) {
    switch (code) {
    case ReqCode::GetTime:
        return GetTime();
    case ReqCode::GetTimeWithoutDate:
        return GetTimeWithoutDate();
    case ReqCode::GetTimeSinceEpoch:
        return GetTimeSinceEpoch();
    case ReqCode::GetClientToServerDelayEstimation:
        return GetClientToServerDelayEstimation();
    case ReqCode::MeasuureRTT:
        return MeasureRTT();
    case ReqCode::GetTimeWithoutDateOrSeconds:
        return GetTimeWithoutDateOrSeconds();
    case ReqCode::GetYear:
        return GetYear();
    case ReqCode::GetMonthAndDay:
        return GetMonthAndDay();
    case ReqCode::GetSecondsSinceBeginningOfMonth:
        return GetSecondsSinceBeginningOfMonth();
    case ReqCode::GetWeekOfYear:
        return GetWeekOfYear();
    case ReqCode::GetDaylightSavings:
        return GetDaylightSavings();
    case ReqCode::GetTimeWithoutDateInCity:
        return GetTimeWithoutDateInCity();
    case ReqCode::MeasureTimeLap:
        return MeasureTimeLap();
    }
    return false;
}

/**
 * @brief Encodes a Request object into a vector of bytes for sending.
 * @param request_ Request object.
 * @return Encoded request as vector<char>.
 */
std::vector<char> TimeClient::incode(const TimeClient::Request& request_) {
    std::vector<char> request;
    request.push_back(static_cast<char>(request_.code));
    for (const auto& arg : request_.args) {
        request.push_back('\0'); // Null separator between arguments
        request.insert(request.end(), arg.begin(), arg.end());
    }
    return request;
}

/**
 * @brief Checks if the response indicates an error.
 * @param response Response vector.
 * @return true if error, false otherwise.
 */
bool TimeClient::isError(const std::vector<char>& response) {
    return (response.empty() || static_cast<ReqCode>(response[0]) == ReqCode::Error);
}

/**
 * @brief Gets the current time and date from the server.
 * @return true if successful, false otherwise.
 */
bool TimeClient::GetTime() {
    if (!sendRequest(ReqCode::GetTime)) {
        return false;
    }
    std::string response;
    if (!receiveResponse(response)) {
        return false;
    }
    std::cout << "The time and date are: " << response << std::endl;
    return true;
}

/**
 * @brief Gets the current time (without date) from the server.
 * @return true if successful, false otherwise.
 */
bool TimeClient::GetTimeWithoutDate() {
    if (!sendRequest(ReqCode::GetTimeWithoutDate)) {
        return false;
    }
    std::string response;
    if (!receiveResponse(response)) {
        return false;
    }
    std::cout << "The time is: " << response << std::endl;
    return true;
}

/**
 * @brief Gets the seconds since epoch from the server.
 * @return true if successful, false otherwise.
 */
bool TimeClient::GetTimeSinceEpoch() {
    if (!sendRequest(ReqCode::GetTimeSinceEpoch)) {
        return false;
    }
    uint32_t response;
    if (!receiveResponse(response)) {
        return false;
    }
    std::cout << "Seconds since epoch: " << response << std::endl;
    return true;
}

/**
 * @brief Estimates client-to-server delay by collecting 100 samples.
 * @return true if successful, false otherwise.
 */
bool TimeClient::GetClientToServerDelayEstimation() {
    std::vector<uint32_t> samples;
    samples.reserve(100);
    double avg = 0.0;

    // Send 100 requests
    for (int i = 0; i < 100; ++i) {
        if (!sendRequest(ReqCode::GetClientToServerDelayEstimation)) return false;
    }
    // Receive 100 responses
    for (int i = 0; i < 100; ++i) {
        uint32_t sample;
        if (!receiveResponse(sample)) return false;
        samples.push_back(sample);
    }
    avg = calcAvgDifference(samples);
    std::cout << "Average client-to-server delay: " << avg << " ms\n";
    return true;
}

/**
 * @brief Measures round-trip time (RTT) by sending 100 requests and timing each.
 * @return true if successful, false otherwise.
 */
bool TimeClient::MeasureRTT() {
    double sum = 0.0;
    for (int i = 0; i < 100; ++i) {
        std::string response;
        DWORD t0 = GetTickCount();
        if (!sendRequest(ReqCode::MeasuureRTT)) return false;
        if (!receiveResponse(response)) return false;
        DWORD t1 = GetTickCount();
        sum += static_cast<double>(t1 - t0);
    }
    double avg = sum / 100.0;
    std::cout << "Average round-trip time (RTT): " << avg << " ms\n";
    return true;
}

/**
 * @brief Gets the current time (without date or seconds) from the server.
 * @return true if successful, false otherwise.
 */
bool TimeClient::GetTimeWithoutDateOrSeconds() {
    if (!sendRequest(ReqCode::GetTimeWithoutDateOrSeconds)) {
        return false;
    }
    std::string response;
    if (!receiveResponse(response)) {
        return false;
    }
    std::cout << "The time is: " << response << std::endl;
    return true;
}

/**
 * @brief Gets the current year from the server.
 * @return true if successful, false otherwise.
 */
bool TimeClient::GetYear() {
    if (!sendRequest(ReqCode::GetYear)) {
        return false;
    }
    std::string response;
    if (!receiveResponse(response)) {
        return false;
    }
    std::cout << "The year is: " << response << std::endl;
    return true;
}

/**
 * @brief Gets the current month and day from the server.
 * @return true if successful, false otherwise.
 */
bool TimeClient::GetMonthAndDay() {
    if (!sendRequest(ReqCode::GetMonthAndDay)) {
        return false;
    }
    std::string response;
    if (!receiveResponse(response)) {
        return false;
    }
    std::cout << "The month and day are: " << response << std::endl;
    return true;
}

/**
 * @brief Gets the seconds since the beginning of the month from the server.
 * @return true if successful, false otherwise.
 */
bool TimeClient::GetSecondsSinceBeginningOfMonth() {
    if (!sendRequest(ReqCode::GetSecondsSinceBeginningOfMonth)) {
        return false;
    }
    uint32_t response;
    if (!receiveResponse(response)) {
        return false;
    }
    std::cout << "Seconds since beginning of month: " << response << std::endl;
    return true;
}

/**
 * @brief Gets the week of the year from the server.
 * @return true if successful, false otherwise.
 */
bool TimeClient::GetWeekOfYear() {
    if (!sendRequest(ReqCode::GetWeekOfYear)) {
        return false;
    }
    uint32_t response;
    if (!receiveResponse(response)) {
        return false;
    }
    std::cout << "Week of the year: " << response << std::endl;
    return true;
}

/**
 * @brief Gets daylight savings status from the server.
 * @return true if successful, false otherwise.
 */
bool TimeClient::GetDaylightSavings() {
    if (!sendRequest(ReqCode::GetDaylightSavings)) {
        return false;
    }
    std::string response;
    if (!receiveResponse(response)) {
        return false;
    }
    std::cout << "It is currently " << ((response == "1") ? "Daylight Saving Time" : "Standard Time") << "." << std::endl;
    return true;
}

/**
 * @brief Gets the current time in a specified city from the server.
 * @return true if successful, false otherwise.
 */
bool TimeClient::GetTimeWithoutDateInCity() {
    std::string city = promptCity(); // Ask user for city name
    Request request(ReqCode::GetTimeWithoutDateInCity, {city});
    if (!sendRequest(request)) {
        std::cout << "Failed to send request for city: " << city << std::endl;
        return false;
    }
    std::string response;
    if (!receiveResponse(response)) {
        std::cout << "Failed to receive response for city: " << city << std::endl;
        return false;
    }
    std::cout << "The time in " << city << " is: " << response << std::endl;
    return true;
}

/**
 * @brief Measures time lap (start/stop timer) using the server.
 * @return true if successful, false otherwise.
 */
bool TimeClient::MeasureTimeLap() {
    if (!sendRequest(ReqCode::MeasureTimeLap)) {
        std::cout << "Failed to send start lap request.\n";
        return false;
    }
    std::string response;
    if (!receiveResponse(response)) {
        std::cout << "Failed to receive lap time.\n";
        return false;
    }
    if (response == "Timer started") {
        std::cout << "Timer started. Send the same request again to stop the timer." << std::endl;
    }
    else {
        std::cout << "Time elapsed since the timer was started: " << response << std::endl;
    }
    return true;
}