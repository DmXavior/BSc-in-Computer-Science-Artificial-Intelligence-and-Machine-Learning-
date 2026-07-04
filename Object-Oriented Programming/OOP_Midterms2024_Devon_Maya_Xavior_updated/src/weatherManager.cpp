#include "weatherManager.h"
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
#include <iostream>

// To store column headers for identifying country codes
std::vector<std::string> headers;

// ------------------------------------
// 1) Load raw weather data from CSV
// ------------------------------------
void weatherManager::loadData(const std::string& filePath) {
    dataFilePath = filePath; // Save the file path
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return;
    }

    std::string line;
    bool isFirstLine = true;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string value;

        if (isFirstLine) {
            // Extract headers from the first line
            std::getline(ss, value, ','); // Skip "utc_timestamp"
            while (std::getline(ss, value, ',')) {
                headers.push_back(value);
            }
            isFirstLine = false;
            continue;
        }

        WeatherData row;
        std::getline(ss, row.timestamp, ','); // First column is the timestamp

        // Remaining columns are temperatures for different countries
        while (std::getline(ss, value, ',')) {
            try {
                row.temperatures.push_back(std::stod(value));
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid temperature value in file. Setting to 0.\n";
                row.temperatures.push_back(0.0); // Handle invalid data gracefully
            }
        }

        weatherData.push_back(row);
    }

    std::cout << "Data loaded successfully from " << filePath
              << ". Total rows: " << weatherData.size() << std::endl;
}

// ------------------------------------
// 2) Access the loaded weather data
// ------------------------------------
const std::vector<WeatherData>& weatherManager::getWeatherData() const {
    return weatherData;
}

// ------------------------------------------------------
// 3) Show a preview of the first 'n' rows of raw data
// ------------------------------------------------------
void weatherManager::showPreview(size_t rows) const {
    size_t count = 0;
    for (const auto& data : weatherData) {
        std::cout << "Timestamp Preview: " << data.timestamp << ", Temperatures: ";
        for (double temp : data.temperatures) {
            std::cout << temp << " ";
        }
        std::cout << std::endl;

        if (++count >= rows) break;
    }
}

// ------------------------------------------------------
// 4) Get Unique Country Codes from Headers
// ------------------------------------------------------
std::set<std::string> weatherManager::getCountryCodes() const {
    std::set<std::string> countryCodes;

    std::ifstream file(dataFilePath);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open the file for extracting headers." << std::endl;
        return countryCodes;
    }

    std::string line;
    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string header;

        // Skip the first column header ("utc_timestamp")
        std::getline(ss, header, ',');

        // Collect remaining headers as country codes
        while (std::getline(ss, header, ',')) {
            countryCodes.insert(header);
        }
    }

    return countryCodes;
}

// ------------------------------------------------------
// 5) Filter Raw Weather Data by Country
// ------------------------------------------------------
std::vector<WeatherData> weatherManager::getRawWeatherDataByCountry(const std::string& countryCode) const {
    std::vector<WeatherData> filteredData;

    // Check if the country code exists in the headers
    auto it = std::find(headers.begin(), headers.end(), countryCode);
    if (it == headers.end()) {
        std::cerr << "Error: Invalid country code '" << countryCode << "' provided.\n";
        return filteredData;
    }

    size_t countryIndex = std::distance(headers.begin(), it);

    // Filter data for the selected country
    for (const auto& data : weatherData) {
        if (countryIndex < data.temperatures.size()) {
            WeatherData filteredRow = data;
            filteredRow.temperatures = { data.temperatures[countryIndex] }; // Keep only the temperature for the selected country
            filteredData.push_back(filteredRow);
        }
    }

    return filteredData;
}

// ------------------------------------------------------
// 6) Filter Raw Weather Data by Date Range
// ------------------------------------------------------
std::vector<WeatherData> weatherManager::getRawWeatherDataByDateRange(const std::string& startDate, const std::string& endDate) const {
    std::vector<WeatherData> filteredData;

    // Iterate over the weather data
    for (const auto& data : weatherData) {
        if (data.timestamp >= startDate && data.timestamp <= endDate) {
            filteredData.push_back(data); // Add rows within the date range
        }
    }

    return filteredData;
}

// ------------------------------------------------------
// 7) Compute Candlestick Data
// ------------------------------------------------------
void weatherManager::computeCandlestickData(const std::string& selectedCountryCode) {
    candlestickData.clear();
    std::map<int, std::vector<double>> yearToTemps;

    // Find the index of the selected country code
    auto it = std::find(headers.begin(), headers.end(), selectedCountryCode);
    if (it == headers.end() && selectedCountryCode != "global") {
        std::cerr << "Error: Invalid country code '" << selectedCountryCode << "' provided.\n";
        return;
    }

    size_t columnIndex = std::distance(headers.begin(), it);

    // Filter temperatures by year and country code
    for (const auto& row : weatherData) {
        if (selectedCountryCode == "global") {
            for (double temp : row.temperatures) {
                int year = std::stoi(row.timestamp.substr(0, 4));
                yearToTemps[year].push_back(temp);
            }
        } else {
            if (columnIndex < row.temperatures.size()) {
                int year = std::stoi(row.timestamp.substr(0, 4));
                yearToTemps[year].push_back(row.temperatures[columnIndex]);
            }
        }
    }

    // Compute candlestick data for each year
    double prevClose = 0.0; // Default for the first year
    bool isFirstYear = true;

    for (const auto& [year, temps] : yearToTemps) {
        if (temps.empty()) continue;

        double high = *std::max_element(temps.begin(), temps.end());
        double low = *std::min_element(temps.begin(), temps.end());
        double close = std::accumulate(temps.begin(), temps.end(), 0.0) / temps.size();

        double open;
        if (isFirstYear) {
            open = temps.front(); // Use first temperature for the first year
            isFirstYear = false;
        } else {
            open = prevClose; // Set Open to the previous year's Close
        }

        // Update prevClose for the next iteration
        prevClose = close;

        candlestickData.emplace_back(std::to_string(year), open, high, low, close);
    }

    std::cout << "Candlestick data computed for "
              << (selectedCountryCode == "global" ? "global" : selectedCountryCode)
              << ". Total candlesticks: " << candlestickData.size() << "\n";
}

// ------------------------------------------------------
// 8) Filter Candlestick Data by Country
// ------------------------------------------------------
std::vector<Candlestick> weatherManager::getFilteredCandlestickDataByCountry(const std::string& countryCode) const {
    std::vector<Candlestick> filteredCandlesticks;

    // Check if the country code exists in the headers
    auto it = std::find(headers.begin(), headers.end(), countryCode);
    if (it == headers.end()) {
        std::cerr << "Error: Invalid country code '" << countryCode << "' provided.\n";
        return filteredCandlesticks;
    }

    size_t countryIndex = std::distance(headers.begin(), it);

    // Filter candlesticks for the selected country
    for (const auto& candle : candlestickData) {
        if (countryCode == "global" || headers[countryIndex] == countryCode) {
            filteredCandlesticks.push_back(candle);
        }
    }

    return filteredCandlesticks;
}

// ------------------------------------------------------
// 9) Filter Candlestick Data by Date Range
// ------------------------------------------------------
std::vector<Candlestick> weatherManager::getFilteredCandlestickDataByDateRange(int startYear, int endYear) const {
    std::vector<Candlestick> filteredData;
    for (const auto& candle : candlestickData) {
        int year = std::stoi(candle.getDate());
        if (year >= startYear && year <= endYear) {
            filteredData.push_back(candle);
        }
    }
    return filteredData;
}

// ------------------------------------------------------
// 10) Get Computed Candlestick Data
// ------------------------------------------------------
const std::vector<Candlestick>& weatherManager::getCandlestickData() const {
    return candlestickData;
}

// ------------------------------------------------------
// 11) Helper for Averages
// ------------------------------------------------------
std::string extractDatePart(const std::string& timestamp, const std::string& granularity) {
    if (granularity == "day" || granularity == "daily") {
        if (timestamp.size() >= 10) return timestamp.substr(0, 10); // YYYY-MM-DD
    }
    if (granularity == "month" || granularity == "monthly") {
        if (timestamp.size() >= 7) return timestamp.substr(0, 7); // YYYY-MM
    }
    if (granularity == "year" || granularity == "yearly") {
        if (timestamp.size() >= 4) return timestamp.substr(0, 4); // YYYY
    }
    std::cerr << "Error: Invalid timestamp or granularity: " << timestamp << ", " << granularity << "\n";
    return "";
}

// Generic function to compute averages
std::map<std::string, double> weatherManager::computeAverages(
    const std::string& countryCode, const std::string& startDate, const std::string& endDate, const std::string& granularity) const {
    std::map<std::string, std::vector<double>> groupedTemps;
    size_t countryIndex = 0;

    if (countryCode != "global") {
        auto it = std::find(headers.begin(), headers.end(), countryCode);
        if (it == headers.end()) {
            std::cerr << "Error: Invalid country code '" << countryCode << "' provided.\n";
            return {};
        }
        countryIndex = std::distance(headers.begin(), it);
    }

    std::cout << "Processing weather data for country code '" << countryCode 
              << "' between '" << startDate << "' and '" << endDate << "'\n";

    for (const auto& row : weatherData) {
        if (row.timestamp < startDate || row.timestamp > endDate) continue;

        std::string key = extractDatePart(row.timestamp, granularity);
        if (key.empty()) continue;

        if (countryCode == "global") {
            groupedTemps[key].insert(groupedTemps[key].end(), row.temperatures.begin(), row.temperatures.end());
        } else if (countryIndex < row.temperatures.size()) {
            groupedTemps[key].push_back(row.temperatures[countryIndex]);
        }
    }

    std::map<std::string, double> averages;
    for (const auto& [key, temps] : groupedTemps) {
        if (!temps.empty()) {
            averages[key] = std::accumulate(temps.begin(), temps.end(), 0.0) / temps.size();
            std::cout << "Average for " << key << " = " << averages[key] << "\n";
        }
    }

    if (averages.empty()) {
        std::cerr << "No averages computed for the selected range and granularity.\n";
    }

    return averages;
}

// Daily averages
std::map<std::string, double> weatherManager::computeDailyAverages(
    const std::string& countryCode, const std::string& startDate, const std::string& endDate) const {
    return computeAverages(countryCode, startDate, endDate, "day");
}

// Monthly averages
std::map<std::string, double> weatherManager::computeMonthlyAverages(
    const std::string& countryCode, const std::string& startDate, const std::string& endDate) const {
    return computeAverages(countryCode, startDate, endDate, "month");
}

// Yearly averages
std::map<std::string, double> weatherManager::computeYearlyAverages(
    const std::string& countryCode, const std::string& startDate, const std::string& endDate) const {
    return computeAverages(countryCode, startDate, endDate, "year");
}