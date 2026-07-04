#include "menu.h"
#include <iostream>
#include "weatherManager.h"
#include "candlestickData.h"
#include "plotData.h"
#include "filterData.h"
#include "predictData.h"

bool dataLoaded = false; // Variable to track if data has been loaded
weatherManager manager;  // Global instance of weatherManager

void showMenu() {
    std::cout << "===========================================\n";
    std::cout << "       Weather Analysis Toolkit\n";
    std::cout << "===========================================\n";
    std::cout << "Please choose an option from the menu below:\n";
    std::cout << "1. Load Data - Please load data to proceed with below options\n";
    std::cout << "2. Compute Candlestick Data\n";
    std::cout << "3. Create Text-Based Candlestick Plot\n";
    std::cout << "4. Filter Data\n";
    std::cout << "5. Predict Weather Data\n";
    std::cout << "6. Help\n";
    std::cout << "7. Exit\n";
}

void showHelp() {
    std::cout << "Help Menu:\n";
    std::cout << "1. Load Data: Load weather data from a file.\n";
    std::cout << "2. Compute Candlestick Data: Generate Yearly Candlestick Data (Open|Close|High|Low) - Global or Country-Specific.\n";
    std::cout << "3. Create Text-Based Candlestick Plot: 5-year range.\n";
    std::cout << "4. Filter Data: Filter the loaded data based on criteria - Country, Date Range or Averages.\n";
    std::cout << "5. Predict Weather Data: Predict future temperature changes.\n";
    std::cout << "6. Help: Show this help menu.\n";
    std::cout << "7. Exit: Close the application.\n";
}

void handleMenuChoice(int choice) {
    switch (choice) {
        case 1:
            if (dataLoaded) {
                std::cout << "Data has already been loaded. Would you like to reload from a different file? (y/n): ";
                char reloadChoice;
                std::cin >> reloadChoice;
                if (reloadChoice == 'y' || reloadChoice == 'Y') {
                    manager.clearData();  // Clear the current data
                    dataLoaded = false;  // Reset the dataLoaded flag
                    std::cout << "Data cleared. Please enter the path to the new weather data file: ";
                } else {
                    std::cout << "Returning to menu...\n";
                    break;
                }
            }

            if (!dataLoaded) {
                std::string filePath;
                std::cout << "Enter the path to the weather data file: ";
                std::cin >> filePath;

                manager.loadData(filePath); // Use weatherManager to load data
                dataLoaded = true;

                // Show the first 5 rows of data after loading
                manager.showPreview();
            }
            break;

        case 2: {
            std::cout << "Would you like to see data for:\n";
            std::cout << "1. Global (all countries)\n";
            std::cout << "2. A specific country\n";
            std::cout << "Enter your choice (1 or 2): ";
            int subChoice;
            std::cin >> subChoice;

            std::string selectedCountryCode = "global";

            if (subChoice == 2) {
                std::cout << "Available country codes from the data file:\n";
                std::set<std::string> countryCodes = manager.getCountryCodes();
                for (const auto& code : countryCodes) {
                    std::cout << code << "\n";
                }

                std::cout << "Enter the country code to analyze: ";
                std::cin >> selectedCountryCode;
            }

            manager.computeCandlestickData(selectedCountryCode);

            std::cout << "Would you like to view the computed candlestick data now? (y/n): ";
            char viewChoice;
            std::cin >> viewChoice;

            if (viewChoice == 'y' || viewChoice == 'Y') {
                const auto& candlesticks = manager.getCandlestickData();
                if (candlesticks.empty()) {
                    std::cout << "No candlestick data available.\n";
                } else {
                    std::cout << "------ Candlestick Details ------\n";
                    for (const auto& c : candlesticks) {
                        c.print();
                    }
                }
            } else {
                std::cout << "Returning to main menu without displaying candlestick data...\n";
            }
            break;
        }

        case 3:
            plotData();
            break;

        case 4: {
            std::cout << "Choose a filter type:\n";
            std::cout << "1. Filter by Country\n";
            std::cout << "2. Filter by Date Range\n";
            std::cout << "3. Compute Averages\n";
            std::cout << "Enter your choice: ";
            int filterChoice;
            std::cin >> filterChoice;

            if (filterChoice == 1) {
                std::cout << "Available country codes from the data file:\n";
                std::set<std::string> countryCodes = manager.getCountryCodes();
                for (const auto& code : countryCodes) {
                    std::cout << code << "\n";
                }

                std::string selectedCountryCode;
                std::cout << "Enter the country code to filter by: ";
                std::cin >> selectedCountryCode;

                const auto& filteredData = manager.getRawWeatherDataByCountry(selectedCountryCode);
                if (filteredData.empty()) {
                    std::cout << "No data available for the selected country.\n";
                } else {
                    std::cout << "Filtered Data for " << selectedCountryCode << ":\n";
                    for (const auto& row : filteredData) {
                        std::cout << "Timestamp: " << row.timestamp << ", Temperature: " << row.temperatures.front() << "\n";
                    }
                }
            } else if (filterChoice == 2) {
                std::string startDate, endDate;
                std::cout << "Enter the start date (YYYY-MM-DD): ";
                std::cin >> startDate;
                std::cout << "Enter the end date (YYYY-MM-DD): ";
                std::cin >> endDate;

                const auto& filteredData = manager.getRawWeatherDataByDateRange(startDate, endDate);
                if (filteredData.empty()) {
                    std::cout << "No data available for the specified date range.\n";
                } else {
                    std::cout << "Filtered Weather Data:\n";
                    for (const auto& row : filteredData) {
                        std::cout << "Timestamp: " << row.timestamp << ", Temperatures: ";
                        for (double temp : row.temperatures) {
                            std::cout << temp << " ";
                        }
                        std::cout << "\n";
                    }
                }
            } else if (filterChoice == 3) {
                std::cout << "Would you like to compute averages for:\n";
                std::cout << "1. Global data\n";
                std::cout << "2. A specific country\n";
                std::cout << "Enter your choice (1 or 2): ";
                int subChoice;
                std::cin >> subChoice;

                std::string selectedCountryCode = "global";

                if (subChoice == 2) {
                    std::cout << "Available country codes:\n";
                    std::set<std::string> countryCodes = manager.getCountryCodes();
                    for (const auto& code : countryCodes) {
                        std::cout << code << "\n";
                    }

                    std::cout << "Enter the country code to analyze: ";
                    std::cin >> selectedCountryCode;
                }

                std::cout << "Choose the granularity for averages:\n";
                std::cout << "1. Daily\n";
                std::cout << "2. Monthly\n";
                std::cout << "3. Yearly\n";
                std::cout << "Enter your choice: ";
                int granularityChoice;
                std::cin >> granularityChoice;

                std::string granularity;
                if (granularityChoice == 1) granularity = "daily";
                else if (granularityChoice == 2) granularity = "monthly";
                else if (granularityChoice == 3) granularity = "yearly";
                else {
                    std::cout << "Invalid granularity choice. Returning to menu...\n";
                    break;
                }

                std::string startDate, endDate;
                std::cout << "Enter the start date (YYYY-MM-DD): ";
                std::cin >> startDate;
                std::cout << "Enter the end date (YYYY-MM-DD): ";
                std::cin >> endDate;

                auto averages = manager.computeAverages(selectedCountryCode, startDate, endDate, granularity);
                if (averages.empty()) {
                    std::cout << "No averages computed for the selected range and granularity.\n";
                } else {
                    std::cout << "Averages computed successfully. Plotting...\n";
                    plotAveragesAsBarGraph(averages);
                }
            } else {
                std::cout << "Invalid choice. Returning to main menu...\n";
            }
            break;
        }

        case 5: {
            std::cout << "Available country codes for prediction:\n";
            std::set<std::string> countryCodes = manager.getCountryCodes();
            for (const auto& code : countryCodes) {
                std::cout << code << "\n";
            }

            std::string countryCode;
            int startMonth, endMonth, predictionYear;

            std::cout << "Enter the country code for prediction (e.g., AT_temperature): ";
            std::cin >> countryCode;
            std::cout << "Enter the start month for prediction (1-12): ";
            std::cin >> startMonth;
            std::cout << "Enter the end month for prediction (1-12): ";
            std::cin >> endMonth;
            std::cout << "Enter the prediction year (e.g., 2022): ";
            std::cin >> predictionYear;

            double tempDifference = predictTemperatureDifference(countryCode, startMonth, endMonth, predictionYear);
            std::cout << "Predicted temperature difference between "
                    << startMonth << " and " << endMonth
                    << " in " << predictionYear << ": " << tempDifference << "°C\n";

            // Assuming the predicted temperatures for the months are returned by the prediction function
            std::vector<double> predictedTemps = getPredictedTemperatures(countryCode, startMonth, endMonth, predictionYear);

            // Plot the bar graph with the predicted temperatures
            plotTemperatureTimeSeries(startMonth, endMonth, predictedTemps);
            break;
        }

        case 6:
            showHelp();
            break;

        case 7:
            std::cout << "Exiting the application...\n";
            break;

        default:
            std::cout << "Invalid choice. Please try again.\n";
            break;
    }
}