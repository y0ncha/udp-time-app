/**
 * @file utils.h
 * @brief Utility function and type declarations for the UDP time server.
 *
 * This header provides enums and function prototypes for error reporting, data conversion, time formatting, city input normalization, and string manipulation.
 * Compatible with C++14.
 */
#pragma once
#include <ctime>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <mutex>
#include <cctype>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <WinSock2.h>
#include <cstring>

/**
 * @brief Request codes for time server operations.
 */
enum class ReqCode : char {
    Error = -1,              /**< Error or invalid request. */
    Default = 0,             /**< Default value. */
    GetTime,                 /**< Get current date and time. */
    GetTimeWithoutDate,      /**< Get current time (no date). */
    GetTimeSinceEpoch,       /**< Get seconds since Unix epoch. */
    GetClientToServerDelayEstimation, /**< Get tick count for delay estimation. */
    MeasuureRTT,             /**< Measure round-trip time. */
    GetTimeWithoutDateOrSeconds, /**< Get current time (no seconds). */
    GetYear,                 /**< Get current year. */
    GetMonthAndDay,          /**< Get current month and day. */
    GetSecondsSinceBeginningOfMonth, /**< Get seconds since month start. */
    GetWeekOfYear,           /**< Get current week of year. */
    GetDaylightSavings,      /**< Get daylight savings status. */
    GetTimeWithoutDateInCity,/**< Get current time in a specified city. */
    MeasureTimeLap           /**< Measure time lap for a client. */
};

/**
 * @brief Overloads the << operator for ReqCode enum for readable output.
 * @param os Output stream.
 * @param code ReqCode value to print.
 * @return Reference to the output stream.
 */
std::ostream& operator<<(std::ostream& os, ReqCode code);

/**
 * @brief Gets the current time as a string and stores it in the buffer.
 * @param buffer Vector to store the time string.
 */
void getCurrentTimeString(std::vector<char>& buffer);

/**
 * @brief Logs an error message with the location and last Winsock error code.
 * @param where Name of the function or location where the error occurred.
 */
void logError(const char* where);

/**
 * @brief Logs a message with a timestamp.
 * @param message Message to log.
 */
void logMessage(const std::string& message);

// Request handlers for each supported operation
// 1. Get current date and time
std::string GetTime();
// 2. Get current time (no date)
std::string GetTimeWithoutDate();
// 3. Get seconds since epoch
uint32_t GetTimeSinceEpoch();
// 4. Estimate client-to-server delay
uint32_t GetClientToServerDelayEstimation();
// 5. Measure round-trip time (RTT)
std::string MeasureRTT();
// 6. Get time without seconds
std::string GetTimeWithoutDateOrSeconds();
// 7. Get current year
std::string GetYear();
// 8. Get current month and day
std::string GetMonthAndDay();
// 9. Get seconds since beginning of month
uint32_t GetSecondsSinceBeginingOfMonth();
// 10. Get week number of year
uint32_t GetWeekOfYear();
// 11. Get daylight savings status
std::string GetDaylightSavings();
// 12. Get time in another city
std::string GetTimeWithoutDateInCity(const std::string& cityName);

/**
 * @brief Measures the time lap for a client endpoint.
 *        Starts timer on first request, returns elapsed time on second request.
 * @param src_addr_be Source address (big-endian).
 * @param src_port_be Source port (big-endian).
 * @return Elapsed time in MM:SS format, or "Timer started" on first request.
 */
std::string MeasureTimeLap(unsigned long src_addr_be, unsigned short src_port_be);

// Small helpers
/**
 * @brief Converts a time_t to local time (thread-safe).
 * @param t Time value.
 * @return Local time as std::tm.
 */
static std::tm to_local(std::time_t t);

/**
 * @brief Converts a time_t to UTC time (thread-safe).
 * @param t Time value.
 * @return UTC time as std::tm.
 */
static std::tm to_utc(std::time_t t);

/**
 * @brief Formats a std::tm struct as a string using the given pattern.
 * @param tm Time struct.
 * @param pat Format pattern (strftime).
 * @return Formatted time string.
 */
static std::string fmt_tm(const std::tm& tm, const char* pat);

/**
 * @brief Calculates seconds since the beginning of the month.
 * @param now Current time.
 * @return Seconds since month start.
 */
static uint32_t seconds_since_month_start(std::time_t now);

/**
 * @brief Calculates the week number of the year (Sunday-based).
 * @param now Current time.
 * @return Week number (0..53).
 */
static uint32_t week_of_year(std::time_t now);

/**
 * @brief Trims whitespace, converts to lowercase, and replaces spaces with hyphens.
 * @param s String to normalize.
 * @return Normalized string.
 */
static std::string trim_lower(std::string s);

/**
 * @brief Calculates the nth weekday of a month (e.g., 2nd Sunday).
 * @param year Year.
 * @param month Month (1-12).
 * @param weekday Day of week (0=Sun..6=Sat).
 * @param nth Nth occurrence.
 * @return Day of month for nth weekday.
 */
static int nth_weekday_of_month(int year, int month, int weekday, int nth);

/**
 * @brief Calculates the last weekday of a month (e.g., last Sunday).
 * @param year Year.
 * @param month Month (1-12).
 * @param weekday Day of week (0=Sun..6=Sat).
 * @return Day of month for last weekday.
 */
static int last_weekday_of_month(int year, int month, int weekday);

/**
 * @brief Checks if the given UTC time is in EU DST window.
 * @param utc UTC time struct.
 * @return true if DST is active, false otherwise.
 */
static bool is_dst_eu(const std::tm& utc);

/**
 * @brief Checks if the given local time is in US DST window (approximate).
 * @param now_local_base Local time as time_t.
 * @return true if DST is active, false otherwise.
 */
static bool is_dst_us_local_approx(std::time_t now_local_base);

/**
 * @brief Gets the current time in a specified city, considering DST.
 * @param city_name City name or code.
 * @return Time string in HH:MM:SS format for the city.
 */
static std::string time_in_city(std::string city_name);

/**
 * @brief Converts a uint32_t value to a vector of bytes (network order, no leading zeros).
 * @param val Value to convert.
 * @return Vector of bytes representing the value.
 */
std::vector<char> toBytes(uint32_t val);        