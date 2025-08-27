/**
 * @file server.h
 * @brief TimeServer class and related types for the UDP time server.
 *
 * This header provides the TimeServer class, request struct, and function prototypes for handling UDP-based time requests, socket management, and response dispatching.
 * Compatible with C++14.
 */
#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string>
#include <vector>
#include <iostream>
#include <string.h>
#include "utils.h"

/**
 * @brief Size of the buffer for receiving requests.
 */
static constexpr int BUFFER_SIZE = 255;

/**
 * @brief TimeServer class implements a UDP time server supporting multiple time-related requests.
 */
class TimeServer {
public:
    /**
     * @brief Constructs a TimeServer and initializes Winsock and socket.
     * @param port Port number to bind the server to (default: 27015).
     */
    TimeServer(unsigned short port = 27015);

    /**
     * @brief Destructor. Cleans up resources.
     */
    ~TimeServer();

    /**
     * @brief Main server loop: waits for client requests and dispatches them.
     */
    void run();

    /**
     * @brief Represents a client request, including code and parameters.
     */
    struct Request {
        /**
         * @brief Constructs a Request with default error code and empty parameters.
         */
        Request() : code(ReqCode::Error), params() {}
        ReqCode code;                  /**< Request code indicating the type of request. */
        std::vector<std::string> params; /**< Parameters for the request (e.g., city name). */
    };

private:
    /**
     * @brief Initializes Winsock and binds the UDP socket to the specified port.
     * @return true if initialization succeeds, false otherwise.
     */
    bool initialize();

    /**
     * @brief Cleans up socket and Winsock resources.
     */
    void cleanup();

    /**
     * @brief Receives a request from a client, decodes it, and logs the received data.
     * @param request Reference to a Request object to store the decoded request.
     * @param clientAddr Reference to sockaddr_in to store the client's address.
     * @param clientAddrLen Reference to int to store the length of the client's address.
     * @return true on success, false on error.
     */
    bool receiveRequest(Request& request, sockaddr_in& clientAddr, int& clientAddrLen);

    /**
     * @brief Sends a response to the client using a vector of bytes.
     * @param response Vector of bytes to send.
     * @param clientAddr Client's address.
     * @param clientAddrLen Length of client's address.
     * @return true on success, false on error.
     */
    bool sendResponse(const std::vector<char>& response, const sockaddr_in& clientAddr, int clientAddrLen);

    /**
     * @brief Sends a string response to the client.
     * @param response String to send.
     * @param clientAddr Client's address.
     * @param clientAddrLen Length of client's address.
     * @return true on success, false on error.
     */
    bool sendResponse(const std::string& response, const sockaddr_in& clientAddr, int clientAddrLen);

    /**
     * @brief Sends a uint32_t response to the client.
     * @param response 32-bit unsigned integer to send.
     * @param clientAddr Client's address.
     * @param clientAddrLen Length of client's address.
     * @return true on success, false on error.
     */
    bool sendResponse(uint32_t response, const sockaddr_in& clientAddr, int clientAddrLen);

    /**
     * @brief Decodes a request buffer into a Request struct.
     * @param req Vector of bytes representing the request.
     * @return Decoded Request struct.
     */
    Request decode(std::vector<char> req);

    /**
     * @brief Dispatches the request to the appropriate handler based on the request code and sends the response.
     * @param req The decoded Request object.
     * @param clientAddr Client's address.
     * @param clientAddrLen Length of client's address.
     * @return true if dispatch and response succeed, false otherwise.
     */
    bool dispatch(const TimeServer::Request& req, sockaddr_in clientAddr, int clientAddrLen);

    SOCKET m_socket;              /**< UDP socket for communication. */
    unsigned short m_port;        /**< Port number the server is bound to. */
    sockaddr_in serverAddr_;      /**< Server address structure. */
    bool initialized_;            /**< Indicates if Winsock is initialized. */
};

/**
 * @brief Overloads the << operator for Request struct for logging purposes.
 * @param os Output stream.
 * @param req Request object to print.
 * @return Reference to the output stream.
 */
std::ostream& operator<<(std::ostream& os, const TimeServer::Request& req);

