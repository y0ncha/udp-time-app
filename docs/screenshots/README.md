# Wireshark Screenshots

This directory contains Wireshark packet capture screenshots demonstrating each message type supported by the UDP Time Protocol.

## Screenshot Files

| Filename | Description |
|----------|-------------|
| `gettime.png` | GetTime request/response showing full date and time |
| `gettime_nodate.png` | GetTimeWithoutDate request/response |
| `gettime_epoch.png` | GetTimeSinceEpoch request/response with binary data |
| `delay_estimation.png` | GetClientToServerDelayEstimation request/response |
| `measure_rtt.png` | MeasureRTT request/response (ping/pong) |
| `gettime_noseconds.png` | GetTimeWithoutDateOrSeconds request/response |
| `getyear.png` | GetYear request/response |
| `getmonthday.png` | GetMonthAndDay request/response |
| `seconds_month.png` | GetSecondsSinceBeginningOfMonth request/response |
| `week_of_year.png` | GetWeekOfYear request/response |
| `daylight_savings.png` | GetDaylightSavings request/response |
| `time_in_city.png` | GetTimeWithoutDateInCity request/response with city parameter |
| `time_lap.png` | MeasureTimeLap request/response sequence |

## Capturing Screenshots

To capture these screenshots:

1. Set up the UDP time server and client applications
2. Configure Wireshark to capture on the appropriate network interface
3. Apply filter: `udp.port == 27015`
4. Execute each request type from the client
5. Save packet capture screenshots showing the hex dump and protocol details

## Screenshot Content

Each screenshot should show:
- Request packet with hex dump highlighting the request code and parameters
- Response packet with hex dump showing the response data
- Protocol breakdown in Wireshark's packet details pane
- Clear labeling of source/destination IP addresses and ports