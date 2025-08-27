# UDP Time Protocol Documentation

## Overview

The UDP Time Protocol is a lightweight client-server communication system designed to provide various time-related information services. The system consists of a C++ UDP server that listens for time requests and responds with formatted time data, and a client application that can request different types of time information.

### System Purpose
- Provide real-time clock information in multiple formats
- Support timezone-aware time queries for major cities
- Offer network delay and round-trip time measurements  
- Enable time lap measurement functionality
- Deliver efficient time services over UDP transport

### Transport Protocol
- **Protocol**: User Datagram Protocol (UDP)
- **Default Port**: 27015
- **Message Size**: Maximum 255 bytes
- **Encoding**: Mixed ASCII text and binary data

## Supported Message Types

### 1. GetTime
**Purpose**: Retrieve current date and time in full format

**Client Request**: 
- Request Code: `1` (0x01) - ReqCode::GetTime
- Parameters: None
- Format: `[0x01]`

**Server Response**:
- Format: ASCII string
- Pattern: `DD/MM/YYYY HH:MM:SS`
- Example: `25/12/2023 14:30:45`

**Wireshark Reference**: ![GetTime Request/Response](screenshots/gettime.png)

### 2. GetTimeWithoutDate  
**Purpose**: Retrieve current time without date information

**Client Request**:
- Request Code: `2` (0x02) - ReqCode::GetTimeWithoutDate
- Parameters: None
- Format: `[0x02]`

**Server Response**:
- Format: ASCII string
- Pattern: `HH:MM:SS`
- Example: `14:30:45`

**Wireshark Reference**: ![GetTimeWithoutDate Request/Response](screenshots/gettime_nodate.png)

### 3. GetTimeSinceEpoch
**Purpose**: Retrieve Unix timestamp (seconds since January 1, 1970)

**Client Request**:
- Request Code: `3` (0x03) - ReqCode::GetTimeSinceEpoch
- Parameters: None
- Format: `[0x03]`

**Server Response**:
- Format: Binary integer (network byte order, leading zeros removed)
- Type: uint32_t
- Example: `0x659A8C2D` (1704634413 seconds)

**Wireshark Reference**: ![GetTimeSinceEpoch Request/Response](screenshots/gettime_epoch.png)

### 4. GetClientToServerDelayEstimation
**Purpose**: Retrieve server tick count for network delay estimation

**Client Request**:
- Request Code: `4` (0x04) - ReqCode::GetClientToServerDelayEstimation
- Parameters: None
- Format: `[0x04]`

**Server Response**:
- Format: Binary integer (network byte order, leading zeros removed)
- Type: uint32_t (Windows tick count)
- Example: `0x12345678`

**Wireshark Reference**: ![DelayEstimation Request/Response](screenshots/delay_estimation.png)

### 5. MeasureRTT
**Purpose**: Measure round-trip time between client and server

**Client Request**:
- Request Code: `5` (0x05) - ReqCode::MeasuureRTT
- Parameters: None
- Format: `[0x05]`

**Server Response**:
- Format: Single null character (pong response)
- Pattern: `0x00`
- Note: Client measures RTT by timing request/response cycle

**Wireshark Reference**: ![MeasureRTT Request/Response](screenshots/measure_rtt.png)

### 6. GetTimeWithoutDateOrSeconds
**Purpose**: Retrieve time in hours and minutes only

**Client Request**:
- Request Code: `6` (0x06) - ReqCode::GetTimeWithoutDateOrSeconds
- Parameters: None
- Format: `[0x06]`

**Server Response**:
- Format: ASCII string
- Pattern: `HH:MM`
- Example: `14:30`

**Wireshark Reference**: ![GetTimeNoSeconds Request/Response](screenshots/gettime_noseconds.png)

### 7. GetYear
**Purpose**: Retrieve current year

**Client Request**:
- Request Code: `7` (0x07) - ReqCode::GetYear
- Parameters: None
- Format: `[0x07]`

**Server Response**:
- Format: ASCII string
- Pattern: `YYYY`
- Example: `2023`

**Wireshark Reference**: ![GetYear Request/Response](screenshots/getyear.png)

### 8. GetMonthAndDay
**Purpose**: Retrieve current month and day

**Client Request**:
- Request Code: `8` (0x08) - ReqCode::GetMonthAndDay
- Parameters: None
- Format: `[0x08]`

**Server Response**:
- Format: ASCII string
- Pattern: `DD/MM`
- Example: `25/12`

**Wireshark Reference**: ![GetMonthAndDay Request/Response](screenshots/getmonthday.png)

### 9. GetSecondsSinceBeginningOfMonth
**Purpose**: Retrieve seconds elapsed since the start of current month

**Client Request**:
- Request Code: `9` (0x09) - ReqCode::GetSecondsSinceBeginningOfMonth
- Parameters: None
- Format: `[0x09]`

**Server Response**:
- Format: Binary integer (network byte order, leading zeros removed)
- Type: uint32_t
- Example: `0x00123456` (1193046 seconds)

**Wireshark Reference**: ![SecondsFromMonth Request/Response](screenshots/seconds_month.png)

### 10. GetWeekOfYear
**Purpose**: Retrieve current week number of the year (Sunday-based)

**Client Request**:
- Request Code: `10` (0x0A) - ReqCode::GetWeekOfYear
- Parameters: None
- Format: `[0x0A]`

**Server Response**:
- Format: Binary integer (network byte order, leading zeros removed)
- Type: uint32_t (0-53)
- Example: `0x33` (51st week)

**Wireshark Reference**: ![GetWeekOfYear Request/Response](screenshots/week_of_year.png)

### 11. GetDaylightSavings
**Purpose**: Check if Daylight Saving Time is currently active

**Client Request**:
- Request Code: `11` (0x0B) - ReqCode::GetDaylightSavings
- Parameters: None
- Format: `[0x0B]`

**Server Response**:
- Format: ASCII string
- Pattern: `1` (DST active) or `0` (DST inactive)
- Example: `1`

**Wireshark Reference**: ![GetDaylightSavings Request/Response](screenshots/daylight_savings.png)

### 12. GetTimeWithoutDateInCity
**Purpose**: Retrieve current time for a specific city/timezone

**Client Request**:
- Request Code: `12` (0x0C) - ReqCode::GetTimeWithoutDateInCity
- Parameters: City name (string)
- Format: `[0x0C][0x00][city_name]`
- Example: `[0x0C][0x00]prague`

**Supported Cities**:
- `doha` - Doha, Qatar (UTC+3)
- `prague` - Prague, Czech Republic (UTC+1, DST aware)
- `new-york` - New York, USA (UTC-5, DST aware)
- `berlin` - Berlin, Germany (UTC+1, DST aware)
- `utc` - Coordinated Universal Time (default)

**Server Response**:
- Format: ASCII string
- Pattern: `HH:MM:SS`
- Example: `15:30:45`

**Wireshark Reference**: ![GetTimeInCity Request/Response](screenshots/time_in_city.png)

### 13. MeasureTimeLap
**Purpose**: Measure elapsed time between two requests from the same client

**Client Request**:
- Request Code: `13` (0x0D) - ReqCode::MeasureTimeLap
- Parameters: None
- Format: `[0x0D]`
- Note: Server uses client's IP address and port to track timing

**Server Response**:
- **First Request**: ASCII string `Timer started`
- **Second Request**: ASCII string with elapsed time in `MM:SS` format
- Example: `02:30` (2 minutes, 30 seconds)
- Note: Timer expires after 180 seconds of inactivity

**Wireshark Reference**: ![MeasureTimeLap Request/Response](screenshots/time_lap.png)

## Protocol Message Structure

### Request Message Format
```
+----------+----------+----------+----------+
| ReqCode  |   NULL   | Param1   |   NULL   | ...
|  (1 byte)| (1 byte) | (string) | (1 byte) |
+----------+----------+----------+----------+
```

- **ReqCode**: Single byte containing the request type (see enum values above)
- **Parameters**: Optional null-separated strings for requests that require them
- **Encoding**: ASCII for strings, binary for request codes

### Response Message Format

#### String Responses
```
+----------+----------+----------+
| char[0]  | char[1]  |   ...    |
+----------+----------+----------+
```
- **Encoding**: ASCII text
- **Termination**: No null terminator (UDP length indicates end)

#### Integer Responses  
```
+----------+----------+----------+----------+
| MSB      |          |          | LSB      |
+----------+----------+----------+----------+
```
- **Encoding**: Network byte order (big-endian)
- **Optimization**: Leading zero bytes are removed
- **Type**: uint32_t (1-4 bytes transmitted)

### Error Handling
- Invalid request codes result in no response
- Malformed requests are ignored
- Network errors are handled at the transport layer
- Client timeouts should be implemented for reliability

## Request/Response Examples

### Example 1: Basic Time Request
```
Client → Server: [0x02]                    // GetTimeWithoutDate
Server → Client: "14:30:45"                // Current time response
```

### Example 2: City Time Request  
```
Client → Server: [0x0C][0x00]"prague"      // GetTimeWithoutDateInCity
Server → Client: "15:30:45"                // Prague local time
```

### Example 3: Binary Response
```
Client → Server: [0x03]                    // GetTimeSinceEpoch  
Server → Client: [0x659A8C2D]              // Unix timestamp (4 bytes)
```

### Example 4: Lap Timer Sequence
```
// First request
Client → Server: [0x0D]                    // MeasureTimeLap
Server → Client: "Timer started"           // Timer initialized

// Second request (after 2m 30s)
Client → Server: [0x0D]                    // MeasureTimeLap  
Server → Client: "02:30"                   // Elapsed time
```

## Efficiency and Limitations Analysis

### Protocol Efficiency

**Strengths**:
- **Minimal Overhead**: Single-byte request codes minimize bandwidth usage
- **UDP Transport**: Low latency, no connection overhead
- **Compact Responses**: Binary integers use optimal encoding with leading zero removal
- **Stateless Design**: Each request is independent, enabling high concurrency
- **Small Message Size**: All responses fit within a single UDP packet (< 255 bytes)

**Bandwidth Analysis**:
- Typical request size: 1-20 bytes
- Typical response size: 4-25 bytes  
- Round-trip overhead: ~40 bytes (IP/UDP headers)
- Total bandwidth per query: ~50-85 bytes

### Protocol Limitations

**Reliability Issues**:
- **No Acknowledgment**: UDP provides no delivery guarantees
- **No Retransmission**: Lost packets require client-side retry logic
- **No Error Responses**: Invalid requests are silently ignored
- **Ordering**: No sequence numbers for request/response correlation

**Functional Limitations**:
- **Limited Timezone Support**: Only 5 predefined cities supported
- **No Authentication**: No security or access control mechanisms  
- **Fixed Buffer Size**: 255-byte limit restricts extensibility
- **Platform Dependency**: Windows-specific implementation (Winsock2)
- **Timer State**: Lap timer state is lost on server restart

**Scalability Considerations**:
- **Memory Usage**: Lap timer state grows with unique client endpoints
- **State Management**: Global lap timer map requires synchronization
- **Resource Cleanup**: Timer expiration logic may impact performance

### Recommended Improvements
1. **Add sequence numbers** for request/response correlation
2. **Implement error response codes** for better debugging
3. **Add request/response checksums** for data integrity
4. **Extend timezone database** for broader geographic support
5. **Add authentication mechanism** for secure deployments
6. **Implement graceful error handling** for malformed requests