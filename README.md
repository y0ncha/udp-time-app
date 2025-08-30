# TimeClient & TimeServer

## 1. Project Overview
```
This project simulates a UDP-based time service for Windows.
The client sends various time-related requests (ReqCode 1–13) to the server,
which replies with the requested time information.
Built in C++ using Winsock2 and Visual Studio.
```

## 2. Folder & File Structure
```
Each project has its own folder and the following files:

  TimeClient/
    |- TimeClient.sln     : Visual Studio solution file for the client.
    |- main.cpp           : Program entry point. Initializes and runs TimeClient.
    |- TimeClient.h       : Declaration of the TimeClient class, which manages
                            UDP communication, request construction, and response handling.
    |- TimeClient.cpp     : Definition of TimeClient class methods.
                            Implements client logic and functionality.
    |- utils.h            : Declarations of helper functions for packet formatting
                            and response processing on the client side.
    |- utils.cpp          : Definitions of helper functions for packet building,
                            formatting, and response processing.

  TimeServer/
    |- TimeServer.sln     : Visual Studio solution file for the server.
    |- main.cpp           : Program entry point. Initializes and runs TimeServer.
    |- TimeServer.h       : Declaration of the TimeServer class, which manages
                            UDP communication, request parsing, and time operations.
    |- TimeServer.cpp     : Definition of TimeServer class methods.
                            Implements server logic and functionality.
    |- utils.h            : Declarations of helper functions for packet parsing,
                            time calculations, and formatting on the server side.
    |- utils.cpp          : Definitions of helper functions for packet parsing,
                            time calculations, and response formatting.

main.cpp
  - Contains the main() function for each application.
  - Responsible for creating the main class (TimeClient/TimeServer) and starting the program.

TimeClient.h / TimeServer.h
  - Declare the respective main class and its interface.

TimeClient.cpp / TimeServer.cpp
  - Define the methods for the main class, implementing all core logic.

utils.h / utils.cpp
  - Provide helper functions used throughout each application.
```

## 3. Build Instructions

- Open either TimeClient.sln or TimeServer.sln in Visual Studio.
- Ensure <winsock2.h> is included; link with ws2_32.lib.
- Build and run on Windows.

## 4. Running the Server
```
- Build and run TimeServer.exe.
- The server listens for UDP packets on port 27015 by default.
```

## 5. Running the Client
```
- Build and run TimeClient.exe.
- The client connects to server address 127.0.0.1:27015 by default.
- User selects ReqCode (1–13) to send a time request.
```

## 6. Supported Requests (ReqCodes)
```
  1  : Get full date & time (DD/MM/YYYY HH:MM:SS)
  2  : Get time only (HH:MM:SS)
  3  : Get seconds since Unix epoch
  4  : Get round-trip time (ms)
  5  : Get time without seconds (HH:MM)
  6  : Get day of year (1–366)
  7  : Get day of week (Sunday=0)
  8  : Get month & year (MM/YYYY)
  9  : Get week number in year (ISO week)
  10 : Get local time for a city (city name required)
  11 : Is it summer time? (DST status)
  12 : Get server uptime (seconds since start)
  13 : Get server version/info string
```

## 7. Protocol Notes
```
For details on the UDP packet structure, request/response formats,
and protocol specifications, please refer to the "Time Protocol"
section in appendix.pdf included in this repository.
```
