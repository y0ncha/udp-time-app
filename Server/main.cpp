/**
 * @file main.cpp
 * @brief UDP Time Server Application
 *
 * This program implements a UDP server that listens for time-related requests from clients.
 * The server uses the TimeServer class to handle socket initialization, request decoding,
 * dispatching, and response sending. Supported requests include current time, date, epoch time,
 * delay estimation, and more. All responses are sent back to the client over UDP.
 * Compatible with C++14.
 */

#include "server.h"
#include <cstdlib>
#include <iostream>
#include <ctime>

int main() {
    system("cls");
    TimeServer server;
    server.run();
    return 0;
}
    