#include "plotData.h"
#include "weatherManager.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>

extern weatherManager manager; // Access the global weatherManager instance

// Helper function: Normalize a temperature value to a grid row (0-8)
int normalizeTemperature(double temp, double minTemp, double maxTemp, int rows) {
    return static_cast<int>((temp - minTemp) / (maxTemp - minTemp) * rows);
}

void printCandlestickData(const std::vector<Candlestick>& filteredData) {
    // Print the candlestick data for the selected 5-year range
    std::cout << "\nCandlestick Data for Selected Range:\n";
    std::cout << "Year    Open   High   Low    Close\n";
    std::cout << "=====================================\n";

    for (const auto& candle : filteredData) {
        std::cout << candle.getDate() << " | "
                  << std::setw(6) << candle.getOpen() << " | "
                  << std::setw(6) << candle.getHigh() << " | "
                  << std::setw(6) << candle.getLow() << " | "
                  << std::setw(6) << candle.getClose() << "\n";
    }
}

// Function to print the ASCII candlestick graph
void printCandlestickGraph(const std::vector<Candlestick>& filteredData) {
    const double minTemp = -40.0, maxTemp = 40.0;
    const int rows = 20;  // Number of rows for graphing
    const int candlestickPadding = 5;  // Padding between candlesticks (adjust this value as needed)

    // Print the graph header
    std::cout << "\nCandlestick Graph for Selected Range:\n";
    std::cout << "Temperature (°C)\n";

    // Print the legend
    std::cout << "\nLegend: ";
    std::cout << "O = Open, C = Close, + = High, - = Low, | = Candlestick Body\n";
    std::cout << "-------------------------------------------------------------\n";

    // Loop through each row (y-axis)
    for (int row = rows; row >= 0; --row) {
        double temp = minTemp + row * (maxTemp - minTemp) / rows;
        std::cout << std::setw(3) << static_cast<int>(temp) << " | ";  // Print temperature on y-axis with 3 characters

        bool previousColumnHasCharacter = false;  // Flag to track previous column's character

        // Loop through each candlestick for the selected year range
        for (size_t i = 0; i < filteredData.size(); ++i) {
            // Shift the first candlestick to the right by 2 spaces
            if (i == 0) {
                std::cout << "  ";  // Adding 2 spaces before the first candlestick
            }

            int highRow = normalizeTemperature(filteredData[i].getHigh(), minTemp, maxTemp, rows);
            int lowRow = normalizeTemperature(filteredData[i].getLow(), minTemp, maxTemp, rows);
            int openRow = normalizeTemperature(filteredData[i].getOpen(), minTemp, maxTemp, rows);
            int closeRow = normalizeTemperature(filteredData[i].getClose(), minTemp, maxTemp, rows);

            int updatedCandlestickPadding = candlestickPadding;
            // Plot High to Low with vertical lines
            if (row <= highRow && row >= lowRow) {
                if (row == openRow && row == closeRow) {
                    std::cout << "OC";  // Both Open and Close on the same row
                    updatedCandlestickPadding -= 1;
                } else if (row == openRow) {
                    std::cout << "O";
                } else if (row == closeRow) {
                    std::cout << "C";
                } else if (row == highRow) {
                    std::cout << "+";
                } else if (row == lowRow) {
                    std::cout << "-";
                } else {
                    std::cout << "|";  // Candlestick body
                }
            } else {
                std::cout << " ";
            }

            // Add padding after each candlestick
            for (int p = updatedCandlestickPadding; p > 0; --p) {
                std::cout << " ";
            }
        }
        std::cout << "\n";  // New line after each row
    }

    // Print x-axis (Years)
    std::cout << "      +";
    for (size_t i = 0; i < filteredData.size(); ++i) {
        std::cout << "-----+";  // Adjust the width of the spaces between each year
    }
    std::cout << "\n      ";  // Adjust alignment to match the candlesticks

    // Loop through the filtered data and print the year labels with additional spacing
    for (size_t i = 0; i < filteredData.size(); ++i) {
        std::string year = filteredData[i].getDate().substr(2);  // Get last 2 digits of the year
        if (i == 0) {
            // Add extra space before the first year
            std::cout << " ";
        }
        std::cout << std::setw(4) << year << "  ";  // Print the year with 4 spaces, plus 2 extra spaces - alignment issues (need to fix)
    }
    std::cout << "\n";
}

void plotData() {
    // Get candlestick data from the manager
    const auto& candlesticks = manager.getCandlestickData();

    if (candlesticks.empty()) {
        std::cout << "No candlestick data available for plotting.\n";
        return;
    }

    // Identify the available year ranges
    int startYear = std::stoi(candlesticks.front().getDate());
    int endYear = std::stoi(candlesticks.back().getDate());

    std::cout << "Available candlestick data ranges from " << startYear << " to " << endYear << ".\n";
    std::cout << "Enter the start year for the 5-year range you'd like to view: ";
    int selectedStartYear;
    std::cin >> selectedStartYear;

    // Validate the selected range
    if (selectedStartYear < startYear || selectedStartYear > endYear - 4) {
        std::cout << "Invalid range. Please select a valid start year between " << startYear
                  << " and " << endYear - 4 << ".\n";
        return;
    }

    int selectedEndYear = selectedStartYear + 4;

    std::cout << "You selected the range " << selectedStartYear << " to " << selectedEndYear << ".\n";

    // Filter the candlestick data for the selected range
    std::vector<Candlestick> filteredData;
    for (const auto& candle : candlesticks) {
        int year = std::stoi(candle.getDate());
        if (year >= selectedStartYear && year <= selectedEndYear) {
            filteredData.push_back(candle);
        }
    }

    // Check if any data was found for the range
    if (filteredData.empty()) {
        std::cout << "No candlestick data available for the selected range.\n";
        return;
    }

    // Print the candlestick data for the 5-year range
    printCandlestickData(filteredData);

    // Print the ASCII candlestick graph for the 5-year range
    printCandlestickGraph(filteredData);
}

void plotAveragesAsBarGraph(const std::map<std::string, double>& averages) {
    if (averages.empty()) {
        std::cout << "No averages to plot.\n";
        return;
    }

    // Find the maximum average value for scaling
    double maxAverage = std::max_element(
        averages.begin(), averages.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; }
    )->second;

    const int maxBarLength = 50; // Maximum bar length in characters

    std::cout << "\nGraph (Bar Chart):\n";

    // Draw the bar graph
    for (const auto& [key, value] : averages) {
        int barLength = static_cast<int>((value / maxAverage) * maxBarLength);

        std::cout << key << " | "; // Label (e.g., year/month)
        for (int i = 0; i < barLength; ++i) {
            std::cout << "#"; // Bar character
        }
        std::cout << " (" << std::fixed << std::setprecision(2) << value << ")\n"; // Show value
    }
    std::cout << std::endl;
}

void plotTemperatureTimeSeries(int startMonth, int endMonth, const std::vector<double>& predictedTemps) {
    std::cout << "Predicted average temperatures (" << startMonth << "-" << endMonth << "):\n";
    
    // Temperature range for scaling
    const double minTemp = -10.0;  // Min temperature (adjustable)
    const double maxTemp = 40.0;   // Max temperature (adjustable)
    
    const int maxBarLength = 10; // Maximum number of '*' characters for the bar

    double totalTemp = 0.0;
    int monthsCounted = endMonth - startMonth + 1;

    // Loop through the months and plot the temperatures
    for (int month = startMonth, i = 0; month <= endMonth; ++month, ++i) {
        double temp = predictedTemps[i];
        totalTemp += temp;
        
        // Calculate the number of stars for the bar based on the temperature
        int numStars = static_cast<int>((temp - minTemp) / (maxTemp - minTemp) * maxBarLength);

        // Output the month, stars, and actual temperature
        std::cout << std::setw(3) << month << "  | ";
        for (int j = 0; j < numStars; ++j) {
            std::cout << "*";
        }
        std::cout << "  (" << std::fixed << std::setprecision(1) << temp << "°C)\n";
    }

    // Calculate temperature difference between the first and last month of the range
    double tempDifference = predictedTemps[endMonth - startMonth] - predictedTemps[0];

    std::cout << "\nTemperature difference between month " << startMonth << " and month " << endMonth 
              << " is: " << std::fixed << std::setprecision(1) << tempDifference << "°C\n";
    
    // Print the average temperature for the selected range
    double avgTemp = totalTemp / monthsCounted;
    std::cout << "\nAverage temperature for the selected range (" << startMonth << "-" << endMonth 
              << "): " << std::fixed << std::setprecision(1) << avgTemp << "°C\n";

    std::cout << "Prediction complete.\n";
}