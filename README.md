# UDP Time Application

A UDP-based client-server application that provides various time-related information and measurements. The application consists of a server that handles time requests and a client with an interactive menu system for sending requests.

## Features

### Supported Time Operations
The application supports 13 different time-related operations:

1. **Current date and time** - Full timestamp with date and time
2. **Time only (no date)** - Current time in HH:MM:SS format
3. **Seconds since epoch** - Unix timestamp (seconds since January 1, 1970)
4. **Client-to-server delay estimation** - Network delay measurement
5. **Round-trip time (RTT)** - Complete round-trip network latency
6. **Time without seconds** - Current time in HH:MM format
7. **Current year** - Just the current year
8. **Month and day** - Current month and day
9. **Seconds since month start** - Elapsed seconds from beginning of current month
10. **Week number of year** - Current week number (0-53)
11. **Daylight savings status** - Whether DST is currently active
12. **Time in another city** - Current time in specified timezone
13. **Time lap measurement** - Stopwatch functionality for timing intervals

### Supported Cities/Timezones
- **UTC** - Coordinated Universal Time
- **Doha** - Qatar (UTC+3)
- **Prague** - Czech Republic (UTC+1/+2 with DST)
- **New York** - USA Eastern Time (UTC-5/-4 with DST)
- **Berlin** - Germany (UTC+1/+2 with DST)

## Prerequisites

### System Requirements
- **Windows operating system** (uses Winsock2)
- **C++14 compatible compiler** (Visual Studio 2015 or later recommended)
- **Winsock2 library** (included with Windows)

### Network Requirements
- Available UDP port (default: 27015)
- Network connectivity between client and server

## Building the Application

### Manual Compilation
Since no build system files are provided, compile manually using your preferred C++ compiler:

#### Server
```bash
cd Server
g++ -std=c++14 main.cpp server.cpp utils.cpp -lws2_32 -o TimeServer.exe
```

#### Client
```bash
cd Client
g++ -std=c++14 main.cpp client.cpp utils.cpp -lws2_32 -o TimeClient.exe
```

### Visual Studio
1. Create a new C++ Console Application project
2. Add the respective source files (main.cpp, *.cpp, *.h) to the project
3. Link against `Ws2_32.lib` (already configured via pragma directive)
4. Build the solution

## Usage

### Starting the Server
1. Open a command prompt/terminal
2. Navigate to the Server directory
3. Run the server executable:
   ```
   TimeServer.exe
   ```
4. The server will start listening on port 27015 and display "Time Server: Wait for clients' requests."

### Running the Client
1. Open another command prompt/terminal
2. Navigate to the Client directory
3. Run the client executable:
   ```
   TimeClient.exe
   ```
4. The client will connect to the server (127.0.0.1:27015 by default) and display an interactive menu

### Client Menu Options
```
Select a request type:
===============================

0. Exit
1. Current date and time
2. Time only (no date)
3. Seconds since epoch
4. Client-to-server delay
5. Round-trip time (RTT)
6. Time without seconds
7. Current year
8. Month and day
9. Seconds since month start
10. Week number of year
11. Daylight savings status
12. Time in another city
13. Measure time lap
```

### Usage Examples

#### Getting Current Time
Select option `1` to get the full current date and time:
```
Response: Mon Jan 15 14:30:25 2024
```

#### Getting Time in Another City
Select option `12` and choose from available cities:
```
Select option: 12
Choose a city:
1. UTC
2. Doha
3. Prague
4. New York
5. Berlin
Response: 15:30:25 (Berlin time)
```

#### Measuring Network Delay
Select option `4` to measure client-to-server delay:
```
Response: 2ms (estimated one-way delay)
```

#### Time Lap Measurement
Select option `13` twice to measure elapsed time:
```
First request: Timer started
Second request: 00:15 (15 seconds elapsed)
```

## Architecture

### Communication Protocol
- **Transport**: UDP (User Datagram Protocol)
- **Port**: 27015 (configurable in source)
- **Message Format**: Binary protocol with request codes
- **Request Codes**: Enum-based system for different operations

### Project Structure
```
udp-time-app/
├── Client/           # Client application source
│   ├── main.cpp     # Client entry point
│   ├── client.h     # TimeClient class declaration
│   ├── client.cpp   # TimeClient implementation
│   ├── utils.h      # Client utility functions
│   └── utils.cpp    # Client utility implementations
├── Server/           # Server application source
│   ├── main.cpp     # Server entry point
│   ├── server.h     # TimeServer class declaration
│   ├── server.cpp   # TimeServer implementation
│   ├── utils.h      # Server utility functions
│   └── utils.cpp    # Server utility implementations
├── LICENSE          # MIT License
└── README.md        # This file
```

### Key Classes
- **TimeServer**: Handles UDP socket initialization, request processing, and response sending
- **TimeClient**: Manages client-side communication and user interface
- **Request/Response**: Structured data types for client-server communication

## Technical Details

### Network Configuration
- **Default Server IP**: 127.0.0.1 (localhost)
- **Default Port**: 27015
- **Protocol**: UDP
- **Buffer Size**: 255 bytes

### Time Zone Handling
The application includes sophisticated timezone and daylight saving time calculations:
- EU DST rules for Prague and Berlin
- US DST rules for New York
- Fixed offsets for Doha and UTC

### Error Handling
- Comprehensive Winsock error reporting
- Graceful handling of network failures
- Input validation for user requests

## Troubleshooting

### Common Issues
1. **"WSAStartup failed"**: Ensure Winsock2 is available (Windows requirement)
2. **"bind failed"**: Port 27015 may be in use, try running as administrator
3. **Connection timeout**: Check firewall settings and network connectivity
4. **Compilation errors**: Ensure C++14 support and Winsock2 library linking

### Debug Mode
The client includes a debug flag that can be enabled to show detailed network communication information.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

Copyright (c) 2025 Yonatan Csasznik

## Contributing

This project demonstrates UDP networking and time handling in C++. Feel free to extend the functionality or adapt it for your learning purposes.