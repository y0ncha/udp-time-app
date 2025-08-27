/**
 * @file main.cpp
 * @brief UDP Time Client Application
 *
 * This program implements a UDP client that communicates with a time server.
 * The client connects to a specified server IP and port, then presents a menu-driven
 * interface to the user for sending various time-related requests (such as current time,
 * date, epoch time, delay estimation, and more). Responses from the server are displayed
 * in the console. The client uses the TimeClient class for all networking and protocol logic.
 */

#include "client.h"
#include "utils.h"
#include <iostream>

constexpr int TIME_PORT = 27015;
constexpr const char* SERVER_IP = "127.0.0.1";

int main() {
    TimeClient client(SERVER_IP, TIME_PORT);
    client.run();
    return 0;
}
