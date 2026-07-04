#include "candlestickData.h"
#include "weatherManager.h"
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <map>
#include <string>

// Helper function to extract the year from a timestamp (assuming format "YYYY-MM-DD HH:MM:SS")
std::string extractYear(const std::string& timestamp) {
    return timestamp.substr(0, 4); // First 4 characters represent the year
}

extern weatherManager manager; // Declare manager as extern

void computeCandlestickData() {
    std::cout << "Computing candlestick data...\n";

    // Get weather data from manager
    std::vector<WeatherData> weatherData = manager.getWeatherData();

    std::map<std::string, std::vector<double>> yearlyData; // Map to group temperatures by year

    // Group temperatures by year
    for (const auto& data : weatherData) {
        std::string year = extractYear(data.timestamp);
        yearlyData[year].insert(yearlyData[year].end(), data.temperatures.begin(), data.temperatures.end());
    }

    std::vector<Candlestick> candlestickData;

    // Compute candlestick data for each year
    std::string prevYear = ""; // To track the previous year
    double prevClose = 0.0;    // To compute Open for the current year

    for (const auto& [year, temperatures] : yearlyData) {
        double open = (prevYear.empty()) ? 0.0 : prevClose; // Use previous year's Close as Open
        double close = std::accumulate(temperatures.begin(), temperatures.end(), 0.0) / temperatures.size(); // Average of current year's temperatures
        double high = *std::max_element(temperatures.begin(), temperatures.end()); // Highest temperature
        double low = *std::min_element(temperatures.begin(), temperatures.end());  // Lowest temperature

        // Update prevYear and prevClose for the next iteration
        prevYear = year;
        prevClose = close;

        // Add the candlestick data
        candlestickData.push_back(Candlestick{year, open, high, low, close});
    }

    // Display the candlestick data
    std::cout << "Candlestick Data:\n";
    std::cout << "Date      Open   High   Low    Close\n";
    std::cout << "=====================================\n";
    for (const auto& candlestick : candlestickData) {
        candlestick.print();
    }
}