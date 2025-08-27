/**
 * @file utils.cpp
 * @brief Utility functions for the UDP time client application.
 *
 * This file provides helper functions for error reporting, data conversion,
 * menu display, city input normalization, and string manipulation.
 * All functions are compatible with C++14.
 */

#include "utils.h"

/**
 * @brief Prints a Winsock error message with the given context string.
 * @param context Description of where the error occurred.
 */
void printError(const std::string& context) {
    std::cerr << "Error at " << context << ": code" << WSAGetLastError() << std::endl;
}

/**
 * @brief Calculates the average difference between consecutive samples.
 * @param samples Vector of uint32_t samples.
 * @return Average difference, or 0.0 if not enough samples.
 */
double calcAvgDifference(const std::vector<uint32_t>& samples) {
    if (samples.size() < 2) return 0.0;
    double sum = 0.0;
    for (int i = 1; i < samples.size(); ++i) {
        sum += static_cast<double>(samples[i] - samples[i - 1]);
    }
    return sum / (samples.size() - 1);
}

/**
 * @brief Converts a vector of up to 4 bytes to a uint32_t (network byte order).
 * @param bytes Vector of bytes (size 1-4).
 * @return Converted uint32_t value, or 0 on error.
 */
uint32_t toUint32(const std::vector<char>& bytes) {
    if (bytes.size() > 4 || bytes.empty()) {
        std::cerr << "Invalid byte array size for uint32_t conversion.\n";
        return 0;
    }

    uint32_t val = 0;
    std::vector<char> buf(4, 0);

    // Pad on the left if bytes are less than 4
    size_t offset = bytes.size() < 4 ? 4 - bytes.size() : 0;
    for (size_t i = 0; i < bytes.size() && i < 4; ++i) {
        buf[offset + i] = bytes[i];
    }

    std::memcpy(&val, buf.data(), 4);
    return ntohl(val);  // Convert from network byte order
}

/**
 * @brief Prints the main menu for time requests to the console.
 */
void printMenu() {
    std::cout << "\nSelect a request type:\n";
    std::cout << "===============================\n\n";
    std::cout << "0. Exit\n";
    std::cout << "1. Current date and time\n";
    std::cout << "2. Time only (no date)\n";
    std::cout << "3. Seconds since epoch\n";
    std::cout << "4. Client-to-server delay\n";
    std::cout << "5. Round-trip time (RTT)\n";
    std::cout << "6. Time without seconds\n";
    std::cout << "7. Current year\n";
    std::cout << "8. Month and day\n";
    std::cout << "9. Seconds since month start\n";
    std::cout << "10. Week number of year\n";
    std::cout << "11. Daylight savings status\n";
    std::cout << "12. Time in another city\n";
    std::cout << "13. Measure time lap\n\n";
}

/**
 * @brief Prompts the user to select a city and normalizes the input.
 * @return Normalized city name (e.g., "doha", "prague", "new-york", "berlin", "utc").
 */
std::string promptCity() {
    system("cls");
    std::string city;
    std::cout << "\nChoose a city from the following list:\n";
    std::cout << "=========================================\n\n";
    std::cout << " 1. Doha (Qatar)\n";
    std::cout << " 2. Prague (Czech Republic)\n";
    std::cout << " 3. New-York (USA)\n";
    std::cout << " 4. Berlin (Germany)\n";
    std::cout << " 5. UTC (default)\n";
    std::cout << "\nEnter your choice (no spaces): ";
    std::cin >> city;
    return normalizeCity(city);
}

/**
 * @brief Normalizes a city input string to a standard format.
 *        Accepts city names or numbers, returns canonical city name.
 * @param city_ Input city string.
 * @return Normalized city name.
 */
std::string normalizeCity(std::string& city_) {
    std::string city = trimLower(city_);
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
 * @brief Trims whitespace, converts to lowercase, and replaces spaces with hyphens.
 * @param s Input string.
 * @return Normalized string.
 */
static std::string trimLower(std::string s) {
    auto not_space = [](unsigned char c) { return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
    s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    std::replace(s.begin(), s.end(), ' ', '-'); // Replace spaces with hyphens
    return s;
}