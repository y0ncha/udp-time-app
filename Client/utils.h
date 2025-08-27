/**
 * @file utils.h
 * @brief Utility function and type declarations for the UDP time client.
 *
 * This header provides enums and function prototypes for error reporting,
 * data conversion, menu display, city input normalization, and string manipulation.
 * Compatible with C++14.
 */
#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <winsock2.h>
#include <algorithm>

static constexpr int BUFFER_SIZE = 255; ///< Buffer size for UDP messages

/**
 * @enum ReqCode
 * @brief Request codes for time server operations.
 */
enum class ReqCode : char {
    Error = -1,                      ///< Error or invalid request
    Default = 0,                     ///< Default request
    GetTime,                         ///< Get current date and time
    GetTimeWithoutDate,              ///< Get current time (no date)
    GetTimeSinceEpoch,               ///< Get seconds since epoch
    GetClientToServerDelayEstimation,///< Estimate client-to-server delay
    MeasuureRTT,                     ///< Measure round-trip time (RTT)
    GetTimeWithoutDateOrSeconds,     ///< Get time without seconds
    GetYear,                         ///< Get current year
    GetMonthAndDay,                  ///< Get current month and day
    GetSecondsSinceBeginningOfMonth, ///< Get seconds since month start
    GetWeekOfYear,                   ///< Get week number of year
    GetDaylightSavings,              ///< Get daylight savings status
    GetTimeWithoutDateInCity,        ///< Get time in another city
    MeasureTimeLap                   ///< Measure time lap
};

/**
 * @enum CityCode
 * @brief City codes for time zone selection.
 */
enum class CityCode : char {
    UTC = 0,     ///< UTC time zone
    Doha,        ///< Doha (Qatar)
    Prag,        ///< Prague (Czech Republic)
    NewYork,     ///< New York (USA)
    Berlin,      ///< Berlin (Germany)
};

/**
 * @brief Prints a Winsock error message with the given context string.
 * @param context Description of where the error occurred.
 */
void printError(const std::string& context);

/**
 * @brief Calculates the average difference between consecutive samples.
 * @param samples Vector of uint32_t samples.
 * @return Average difference, or 0.0 if not enough samples.
 */
double calcAvgDifference(const std::vector<uint32_t>& samples);

/**
 * @brief Converts a vector of up to 4 bytes to a uint32_t (network byte order).
 * @param bytes Vector of bytes (size 1-4).
 * @return Converted uint32_t value, or 0 on error.
 */
uint32_t toUint32(const std::vector<char>& bytes);

/**
 * @brief Prints the main menu for time requests to the console.
 */
void printMenu();

/**
 * @brief Prompts the user to select a city and normalizes the input.
 * @return Normalized city name (e.g., "doha", "prague", "new-york", "berlin", "utc").
 */
std::string promptCity();

/**
 * @brief Normalizes a city input string to a standard format.
 *        Accepts city names or numbers, returns canonical city name.
 * @param city Input city string.
 * @return Normalized city name.
 */
std::string normalizeCity(std::string& city);

/**
 * @brief Trims whitespace, converts to lowercase, and replaces spaces with hyphens.
 * @param str Input string.
 * @return Normalized string.
 */
std::string trimLower(std::string str);