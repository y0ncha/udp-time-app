/**
 * @file utils.cpp
 * @brief Implementation of utility functions for the UDP time server.
 *
 * This source file provides utility functions for time formatting, city/timezone handling, logging, and request/response helpers for the UDP time server.
 * Compatible with C++14.
 */

#include "utils.h"
#include <vector>
#include <string>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <iostream>
#include <iomanip>

constexpr size_t BUFFER_SIZE = 255;

/**
 * @brief Overloads the << operator for ReqCode enum for readable output.
 * @param os Output stream.
 * @param code ReqCode value to print.
 * @return Reference to the output stream.
 */
std::ostream& operator<<(std::ostream& os, ReqCode code) {
    switch (code) {
    case ReqCode::Error: return os << "Error";
    case ReqCode::GetTime: return os << "GetTime";
    case ReqCode::GetTimeWithoutDate: return os << "GetTimeWithoutDate";
    case ReqCode::GetTimeSinceEpoch: return os << "GetTimeSinceEpoch";
    case ReqCode::GetClientToServerDelayEstimation: return os << "GetClientToServerDelayEstimation";
    case ReqCode::MeasuureRTT: return os << "MeasuureRTT";
    case ReqCode::GetTimeWithoutDateOrSeconds: return os << "GetTimeWithoutDateOrSeconds";
    case ReqCode::GetYear: return os << "GetYear";
    case ReqCode::GetMonthAndDay: return os << "GetMonthAndDay";
    case ReqCode::GetSecondsSinceBeginningOfMonth: return os << "GetSecondsSinceBeginningOfMonth";
    case ReqCode::GetWeekOfYear: return os << "GetWeekOfYear";
    case ReqCode::GetDaylightSavings: return os << "GetDaylightSavings";
    case ReqCode::GetTimeWithoutDateInCity: return os << "GetTimeWithoutDateInCity";
    case ReqCode::MeasureTimeLap: return os << "MeasureTimeLap";
    default: return os << "Unknown";
    }
}

// ---------- common helpers ----------
/**
 * @brief Gets the current time as a string and stores it in the buffer.
 * @param buffer Vector to store the time string.
 */
void getCurrentTimeString(std::vector<char>& buffer) {
    buffer.resize(BUFFER_SIZE);
    time_t timer;
    time(&timer);
    ctime_s(buffer.data(), buffer.size(), &timer);
    size_t len = strlen(buffer.data());
    if (len > 0 && buffer[len - 1] == '\n')
        buffer[len - 1] = '\0';
    buffer.resize(len > 0 ? len : BUFFER_SIZE);
}

/**
 * @brief Logs an error message with the location and last Winsock error code.
 * @param where Name of the function or location where the error occurred.
 */
void logError(const char* where) {
    std::cout << "Time Server: Error at " << where << "(): " << WSAGetLastError() << std::endl;
}

/**
 * @brief Logs a message with a timestamp.
 * @param message Message to log.
 */
void logMessage(const std::string& message) {
    auto now = std::time(nullptr);
    std::tm tm = to_local(now);
    std::string timestamp = fmt_tm(tm, "%Y-%m-%d %H:%M:%S");
    std::cout << "\n[" << timestamp << "] " << message;
}

// ---------- small helpers ----------
/**
 * @brief Converts a time_t to local time (thread-safe).
 * @param t Time value.
 * @return Local time as std::tm.
 */
static std::tm to_local(std::time_t t) { std::tm out{}; localtime_s(&out, &t); return out; }

/**
 * @brief Converts a time_t to UTC time (thread-safe).
 * @param t Time value.
 * @return UTC time as std::tm.
 */
static std::tm to_utc(std::time_t t) { std::tm out{}; gmtime_s(&out, &t); return out; }

/**
 * @brief Formats a std::tm struct as a string using the given pattern.
 * @param tm Time struct.
 * @param pat Format pattern (strftime).
 * @return Formatted time string.
 */
static std::string fmt_tm(const std::tm& tm, const char* pat) {
    std::vector<char> buf(BUFFER_SIZE, 0);
    std::strftime(buf.data(), buf.size(), pat, &tm);
    return std::string(buf.data());
}

/**
 * @brief Calculates seconds since the beginning of the month.
 * @param now Current time.
 * @return Seconds since month start.
 */
static uint32_t seconds_since_month_start(std::time_t now) {
    std::tm tm = to_local(now);
    tm.tm_mday = 1; tm.tm_hour = 0; tm.tm_min = 0; tm.tm_sec = 0;
    return static_cast<uint32_t>(std::difftime(now, std::mktime(&tm)));
}

/**
 * @brief Calculates the week number of the year (Sunday-based).
 * @param now Current time.
 * @return Week number (0..53).
 */
static uint32_t week_of_year(std::time_t now) {
    std::tm tm = to_local(now);
    std::vector<char> buf(BUFFER_SIZE, 0);
    std::strftime(buf.data(), buf.size(), "%U", &tm); // Sunday-based, 00..53
    uint32_t num = static_cast<uint32_t>(std::atoi(buf.data()));
    return num;
}

/**
 * @brief Trims whitespace, converts to lowercase, and replaces spaces with hyphens.
 * @param s String to normalize.
 * @return Normalized string.
 */
static std::string trim_lower(std::string s) {
    auto not_space = [](unsigned char c) { return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
    s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return std::tolower(c);
        });
    std::replace(s.begin(), s.end(), ' ', '-'); // Replace spaces with hyphens
    return s;
}

/**
 * @brief Normalizes a city name to a canonical form for timezone lookup.
 * @param city_ City name or code.
 * @return Normalized city string.
 */
std::string normalize_city(std::string& city_) {
    std::string city = trim_lower(city_);
    if (city == "doha" || city == "1") {
        return "doha";
    }
    else if (city == "prague" || city == "2") {
        return "prague";
    }
    else if (city == "new-york" || city == "newyork" || city == "new york" || city == "3") {
        return "new-york";
    }
    else if (city == "berlin" || city == "4") {
        return "berlin";
    }
    else {
        return "utc";
    }
}

/**
 * @brief Daylight Saving Time rule for a city.
 */
enum class DstRule { None, EU, US };

/**
 * @brief City timezone information.
 */
struct CityTz { int base_utc_hours; bool has_dst; DstRule rule; };

// City timezone map
static const std::unordered_map<std::string, CityTz> kCities = {
    {"bangkok", {+7, false, DstRule::None}},
    {"prague",  {+1, true,  DstRule::EU  }},
    {"berlin",  {+1, true,  DstRule::EU  }},
    {"new york",{ -5, true,  DstRule::US  }},
};

/**
 * @brief Calculates the nth weekday of a month (e.g., 2nd Sunday).
 * @param year Year.
 * @param month Month (1-12).
 * @param weekday Day of week (0=Sun..6=Sat).
 * @param nth Nth occurrence.
 * @return Day of month for nth weekday.
 */
static int nth_weekday_of_month(int year, int month, int weekday, int nth) {
    std::tm tm{}; tm.tm_year = year - 1900; tm.tm_mon = month - 1; tm.tm_mday = 1;
    std::mktime(&tm);
    int first = tm.tm_wday; // 0=Sun..6=Sat
    return 1 + ((7 + weekday - first) % 7) + (nth - 1) * 7;
}

/**
 * @brief Calculates the last weekday of a month (e.g., last Sunday).
 * @param year Year.
 * @param month Month (1-12).
 * @param weekday Day of week (0=Sun..6=Sat).
 * @return Day of month for last weekday.
 */
static int last_weekday_of_month(int year, int month, int weekday) {
    std::tm tm{}; tm.tm_year = year - 1900; tm.tm_mon = month; tm.tm_mday = 0; // last of prev
    std::mktime(&tm);
    int last_dom = tm.tm_mday;
    tm.tm_mday = 1; std::mktime(&tm);
    int first = tm.tm_wday;
    int last_wd = (first + (last_dom - 1)) % 7;
    int delta = (7 + last_wd - weekday) % 7;
    return last_dom - delta;
}

/**
 * @brief Checks if the given UTC time is in EU DST window.
 * @param utc UTC time struct.
 * @return true if DST is active, false otherwise.
 */
static bool is_dst_eu(const std::tm& utc) {
    int y = utc.tm_year + 1900;
    int start = last_weekday_of_month(y, 3, 0);   // last Sunday in March, 01:00 UTC
    int end = last_weekday_of_month(y, 10, 0);  // last Sunday in Oct,   01:00 UTC
    int mon = utc.tm_mon + 1, day = utc.tm_mday, h = utc.tm_hour;
    if (mon < 3 || mon > 10) return false;
    if (mon > 3 && mon < 10) return true;
    if (mon == 3)  return (day > start) || (day == start && h >= 1);
    if (mon == 10) return (day < end) || (day == end && h < 1);
    return false;
}

/**
 * @brief Checks if the given local time is in US DST window (approximate).
 * @param now_local_base Local time as time_t.
 * @return true if DST is active, false otherwise.
 */
static bool is_dst_us_local_approx(std::time_t now_local_base) {
    std::tm t = to_utc(now_local_base); // treat as “local” clock for that zone
    int y = t.tm_year + 1900;
    int start = nth_weekday_of_month(y, 3, 0, 2);  // 2nd Sunday Mar, 02:00 local
    int end = nth_weekday_of_month(y, 11, 0, 1); // 1st Sunday Nov, 02:00 local
    int mon = t.tm_mon + 1, day = t.tm_mday, h = t.tm_hour;
    if (mon < 3 || mon > 11) return false;
    if (mon > 3 && mon < 11) return true;
    if (mon == 3)  return (day > start) || (day == start && h >= 2);
    if (mon == 11) return (day < end) || (day == end && h < 2);
    return false;
}

/**
 * @brief Gets the current time in a specified city, considering DST.
 * @param city City name or code.
 * @return Time string in HH:MM:SS format for the city.
 */
static std::string time_in_city(std::string city) {
    city = normalize_city(city);
    std::time_t now = std::time(nullptr);
    std::tm utc_tm = to_utc(now);

    // Add Doha and UTC support to the city map
    static const std::unordered_map<std::string, CityTz> kCitiesExt = {
        {"prague",  {+1, true,  DstRule::EU  }},
        {"berlin",  {+1, true,  DstRule::EU  }},
        {"new-york",{ -5, true,  DstRule::US  }},
        {"doha",    {+3, false, DstRule::None}},
        {"utc",     {0,  false, DstRule::None}}
    };

    auto it = kCitiesExt.find(city);
    if (it == kCitiesExt.end()) {
        return fmt_tm(utc_tm, "%H:%M:%S");
    }

    int offset = it->second.base_utc_hours;
    if (it->second.has_dst) {
        if (it->second.rule == DstRule::EU) {
            if (is_dst_eu(utc_tm)) offset += 1;
        }
        else if (it->second.rule == DstRule::US) {
            if (is_dst_us_local_approx(now + offset * 3600)) offset += 1;
        }
    }
    std::tm city_tm = to_utc(now + offset * 3600);
    return fmt_tm(city_tm, "%H:%M:%S");
}

/**
 * @brief Key for identifying client endpoints for lap timing.
 */
struct EndpointKey { unsigned long addr_be; unsigned short port_be; };

/**
 * @brief Equality operator for EndpointKey.
 * @param a First key.
 * @param b Second key.
 * @return true if keys are equal, false otherwise.
 */
static bool operator==(const EndpointKey& a, const EndpointKey& b) {
    return a.addr_be == b.addr_be && a.port_be == b.port_be;
}

/**
 * @brief Hash function for EndpointKey.
 */
struct EndpointKeyHash {
    size_t operator()(const EndpointKey& k) const noexcept {
        return (static_cast<size_t>(k.addr_be) << 16) ^ k.port_be;
    }
};

// Lap timing storage and mutex
static std::unordered_map<EndpointKey, std::chrono::steady_clock::time_point, EndpointKeyHash> g_lap;
static std::mutex g_lap_mx;

// ---------- Handlers 1..13 ----------
/**
 * @brief Gets the current date and time as a string.
 * @return Date and time string in DD/MM/YYYY HH:MM:SS format.
 */
std::string GetTime() {
    auto now = std::time(nullptr);
    return fmt_tm(to_local(now), "%d/%m/%Y %H:%M:%S");
}

/**
 * @brief Gets the current time (without date) as a string.
 * @return Time string in HH:MM:SS format.
 */
std::string GetTimeWithoutDate() {
    auto now = std::time(nullptr);
    return fmt_tm(to_local(now), "%H:%M:%S");
}

/**
 * @brief Gets the current time since Unix epoch in seconds.
 * @return Seconds since epoch.
 */
uint32_t GetTimeSinceEpoch() {
    uint32_t now = static_cast<uint32_t>(std::time(nullptr));
    return now;
}

/**
 * @brief Gets the current tick count for delay estimation.
 * @return Tick count as uint32_t.
 */
uint32_t GetClientToServerDelayEstimation() {
    uint32_t tickCount = static_cast<uint32_t>(GetTickCount());
    return tickCount;
}

/**
 * @brief Handler for RTT measurement (returns Pong).
 * @return String containing a single null character.
 */
std::string MeasureRTT() {
    return std::string(1, 0); // Pong
}

/**
 * @brief Gets the current time without seconds.
 * @return Time string in HH:MM format.
 */
std::string GetTimeWithoutDateOrSeconds() {
    auto now = std::time(nullptr);
    return fmt_tm(to_local(now), "%H:%M");
}

/**
 * @brief Gets the current year as a string.
 * @return Year string in YYYY format.
 */
std::string GetYear() {
    auto now = std::time(nullptr);
    return fmt_tm(to_local(now), "%Y");
}

/**
 * @brief Gets the current month and day as a string.
 * @return String in DD/MM format.
 */
std::string GetMonthAndDay() {
    auto now = std::time(nullptr);
    return fmt_tm(to_local(now), "%d/%m");
}

/**
 * @brief Gets the number of seconds since the beginning of the month.
 * @return Seconds since month start.
 */
uint32_t GetSecondsSinceBeginingOfMonth() {
    return seconds_since_month_start(std::time(nullptr));
}

/**
 * @brief Gets the current week of the year.
 * @return Week number (0..53).
 */
uint32_t GetWeekOfYear() {
    return week_of_year(std::time(nullptr));
}

/**
 * @brief Gets whether daylight savings is active (1) or not (0).
 * @return "1" if DST is active, "0" otherwise.
 */
std::string GetDaylightSavings() {
    std::tm tm = to_local(std::time(nullptr));
    return (tm.tm_isdst > 0) ? "1" : "0";
}

/**
 * @brief Gets the current time in a specified city.
 * @param cityName City name or code.
 * @return Time string in HH:MM:SS format for the city.
 */
std::string GetTimeWithoutDateInCity(const std::string& cityName) {
    return time_in_city(cityName);
}

/**
 * @brief Measures the time lap for a client endpoint.
 *        Starts timer on first request, returns elapsed time on second request.
 * @param src_addr_be Source address (big-endian).
 * @param src_port_be Source port (big-endian).
 * @return Elapsed time in MM:SS format, or "Timer started" on first request.
 */
std::string MeasureTimeLap(unsigned long src_addr_be, unsigned short src_port_be) {
    using clock = std::chrono::steady_clock;
    EndpointKey key{ src_addr_be, src_port_be };
    auto now = clock::now();
    std::lock_guard<std::mutex> lk(g_lap_mx);

    // Remove all expired measurements (older than 180 seconds)
    for (auto it = g_lap.begin(); it != g_lap.end(); ) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count();
        if (elapsed > 180) {
            it = g_lap.erase(it);
        } else {
            ++it;
        }
    }

    auto it = g_lap.find(key);
    if (it == g_lap.end()) {
        // First request: start measurement
        g_lap[key] = now;
        return "Timer started";
    }
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count();
    g_lap.erase(it); // Remove measurement after second request
    int minutes = static_cast<int>(sec / 60);
    int seconds = static_cast<int>(sec % 60);
    std::ostringstream os;
    os << std::setfill('0') << std::setw(2) << minutes << ":"
       << std::setfill('0') << std::setw(2) << seconds;
    return os.str();
}

/**
 * @brief Converts a uint32_t value to a vector of bytes (network order, no leading zeros).
 * @param val Value to convert.
 * @return Vector of bytes representing the value.
 */
std::vector<char> toBytes(uint32_t val) {
    uint32_t netVal = htonl(val);
    unsigned char buf[sizeof(netVal)];
    std::memcpy(buf, &netVal, sizeof(netVal));

    size_t start = 0;
    while (start < sizeof(buf) && buf[start] == 0) {
        ++start;
    }
    return std::vector<char>(buf + start, buf + sizeof(buf));
}
