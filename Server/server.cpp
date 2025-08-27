/**
 * @file server.cpp
 * @brief Implementation of the UDP time server logic.
 *
 * This source file implements the TimeServer class, including socket initialization, request decoding, dispatching, and response sending for UDP-based time requests.
 * Compatible with C++14.
 */
#include "server.h"

/**
 * @brief Constructs a TimeServer and initializes the server address and socket.
 * @param port Port number to bind the server to.
 */
TimeServer::TimeServer(unsigned short port)
    : m_port(port), m_socket(INVALID_SOCKET), initialized_(false)
{
    memset(&serverAddr_, 0, sizeof(serverAddr_));
    initialize();
}

/**
 * @brief Destructor. Cleans up resources.
 */
TimeServer::~TimeServer() {
    cleanup();
}

/**
 * @brief Initializes Winsock and binds the UDP socket to the specified port.
 * @return true if initialization succeeds, false otherwise.
 */
bool TimeServer::initialize() {
    WSAData wsaData;
    if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        logError("WSAStartup");
        return false;
    }
    initialized_ = true;

    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (INVALID_SOCKET == m_socket) {
        logError("socket");
        cleanup();
        return false;
    }

    serverAddr_.sin_family = AF_INET;
    serverAddr_.sin_addr.s_addr = INADDR_ANY;
    serverAddr_.sin_port = htons(m_port);
    if (SOCKET_ERROR == bind(m_socket, (SOCKADDR *)&serverAddr_, sizeof(serverAddr_))) {
        logError("bind");
        cleanup();
        return false;
    }
    return true;
}

/**
 * @brief Cleans up socket and Winsock resources.
 */
void TimeServer::cleanup() {
    if (m_socket != INVALID_SOCKET) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
    if (initialized_) {
        WSACleanup();
        initialized_ = false;
    }
}

/**
 * @brief Receives a request from a client, decodes it, and logs the received data.
 * @param request Reference to a Request object to store the decoded request.
 * @param clientAddr Reference to sockaddr_in to store the client's address.
 * @param clientAddrLen Reference to int to store the length of the client's address.
 * @return true on success, false on error.
 */
bool TimeServer::receiveRequest(TimeServer::Request& request, sockaddr_in& clientAddr, int& clientAddrLen) {
    std::vector<char> buffer(BUFFER_SIZE);
    clientAddrLen = sizeof(clientAddr);
    int bytesRecv = recvfrom(m_socket, buffer.data(), static_cast<int>(buffer.size()), 0, (sockaddr*)&clientAddr, &clientAddrLen);
    if (SOCKET_ERROR == bytesRecv) {
        logError("recvfrom");
        return false;
    }
    buffer.resize(bytesRecv); // shrink to actual size
    request = decode(buffer);

    std::ostringstream oss;
    oss << "Time Server: Received " << bytesRecv << " bytes" << " | " << request;
    logMessage(oss.str());
    return true;
}

/**
 * @brief Sends a response to the client using a vector of bytes.
 * @param response Vector of bytes to send.
 * @param clientAddr Client's address.
 * @param clientAddrLen Length of client's address.
 * @return true on success, false on error.
 */
bool TimeServer::sendResponse(const std::vector<char>& response, const sockaddr_in& clientAddr, int clientAddrLen) {
    int bytesSent = sendto(m_socket, response.data(), (int)response.size(), 0,
        (const sockaddr*)&clientAddr, clientAddrLen);
    if (SOCKET_ERROR == bytesSent) {
        logError("sendto");
        return false;
    }
    std::ostringstream oss;
    oss << "Time Server: Sent " << bytesSent << " bytes";
    logMessage(oss.str());
    return true;
}

/**
 * @brief Sends a string response to the client.
 * @param response_ String to send.
 * @param clientAddr Client's address.
 * @param clientAddrLen Length of client's address.
 * @return true on success, false on error.
 */
bool TimeServer::sendResponse(const std::string& response_, const sockaddr_in& clientAddr, int clientAddrLen) {
    std::vector<char> response(response_.begin(), response_.end());
    if (sendResponse(response, clientAddr, clientAddrLen)) {
        std::cout << " | " << response_;
        return true;
    }
    return false;
}

/**
 * @brief Sends a uint32_t response to the client.
 * @param response_ 32-bit unsigned integer to send.
 * @param clientAddr Client's address.
 * @param clientAddrLen Length of client's address.
 * @return true on success, false on error.
 */
bool TimeServer::sendResponse(uint32_t response_, const sockaddr_in& clientAddr, int clientAddrLen) {
    std::vector<char> response = toBytes(response_);
    if (sendResponse(response, clientAddr, clientAddrLen)) {
        std::cout << " | " << response_;
        return true;
    }
    return false;
}

/**
 * @brief Decodes a request buffer into a Request struct.
 * @param req Vector of bytes representing the request.
 * @return Decoded Request struct.
 */
TimeServer::Request TimeServer::decode(std::vector<char> req) {
    Request result;
    result.code = req.empty() ? ReqCode::Error : static_cast<ReqCode>(req[0]);
    result.params.clear();

    // Parse null-separated arguments
    size_t i = 1;
    while (i < req.size()) {
        if (req[i] == '\0') {
            ++i;
            size_t start = i;
            while (i < req.size() && req[i] != '\0') ++i;
            if (start < req.size()) {
                result.params.emplace_back(req.begin() + start, req.begin() + i);
            }
        } else {
            ++i;
        }
    }
    return result;
}

/**
 * @brief Dispatches the request to the appropriate handler based on the request code and sends the response.
 * @param req The decoded Request object.
 * @param clientAddr Client's address.
 * @param clientAddrLen Length of client's address.
 * @return true if dispatch and response succeed, false otherwise.
 */
bool TimeServer::dispatch(const TimeServer::Request& req, sockaddr_in clientAddr, int clientAddrLen) {
    switch (req.code) {
    case ReqCode::GetTime:
        return sendResponse(GetTime(), clientAddr, clientAddrLen);
    case ReqCode::GetTimeWithoutDate:
        return sendResponse(GetTimeWithoutDate(), clientAddr, clientAddrLen);
    case ReqCode::GetTimeSinceEpoch:
        return sendResponse(GetTimeSinceEpoch(), clientAddr, clientAddrLen);
    case ReqCode::GetClientToServerDelayEstimation:
        return sendResponse(GetClientToServerDelayEstimation(), clientAddr, clientAddrLen);
    case ReqCode::MeasuureRTT:
        return sendResponse(MeasureRTT(), clientAddr, clientAddrLen);
    case ReqCode::GetTimeWithoutDateOrSeconds:
        return sendResponse(GetTimeWithoutDateOrSeconds(), clientAddr, clientAddrLen);
    case ReqCode::GetYear:
        return sendResponse(GetYear(), clientAddr, clientAddrLen);
    case ReqCode::GetMonthAndDay:
        return sendResponse(GetMonthAndDay(), clientAddr, clientAddrLen);
    case ReqCode::GetSecondsSinceBeginningOfMonth:
        return sendResponse(GetSecondsSinceBeginingOfMonth(), clientAddr, clientAddrLen);
    case ReqCode::GetWeekOfYear:
        return sendResponse(GetWeekOfYear(), clientAddr, clientAddrLen);
    case ReqCode::GetDaylightSavings:
        return sendResponse(GetDaylightSavings(), clientAddr, clientAddrLen);
    case ReqCode::GetTimeWithoutDateInCity:
        // Assumes first parameter is city name
        return sendResponse(GetTimeWithoutDateInCity(req.params[0]), clientAddr, clientAddrLen);
    case ReqCode::MeasureTimeLap:
        // Uses client's address and port for lap measurement
        return sendResponse(MeasureTimeLap(clientAddr.sin_addr.S_un.S_addr, clientAddr.sin_port), clientAddr, clientAddrLen);
    }
    return false;
}

/**
 * @brief Main server loop: waits for client requests and dispatches them.
 */
void TimeServer::run() {
    if (!initialized_) {
        logMessage("Time Server: Not initialized properly.");
        return;
    }
    logMessage("Time Server: Wait for clients' requests.");
    while (true) {
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        Request request;
        receiveRequest(request, clientAddr, clientAddrLen);

        if (!dispatch(request, clientAddr, clientAddrLen)) {
            logMessage("Time Server: Dispatch failed.");
        }
    }
}

/**
 * @brief Overloads the << operator for Request struct for logging purposes.
 * @param os Output stream.
 * @param req Request object to print.
 * @return Reference to the output stream.
 */
std::ostream& operator<<(std::ostream& os, const TimeServer::Request& req) {
    os << req.code;
    if (req.params.empty()) return os << " [No Params]";

/* Request struct logging overload - prints request code and parameters
 * Example: if a request has code 1 and parameters ["param1", "param2"], it prints:
 *          "1, Params: [param1, param2]"
 */
    os << ", Params: [";
    for (size_t i = 0; i < req.params.size(); ++i) {
        os << req.params[i];
        if (i + 1 < req.params.size()) os << ", ";
    }
    os << "]";
    return os;
}
