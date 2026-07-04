#include "filterData.h"
#include "weatherManager.h"
#include <iostream>
#include <set>
#include <string>

extern weatherManager manager; // Access the global weatherManager instance

void filterData() {
    std::cout << "Filtering data by country...\n";

    // Display available country codes
    std::set<std::string> countryCodes = manager.getCountryCodes();
    if (countryCodes.empty()) {
        std::cout << "No country codes available. Please ensure data is loaded.\n";
        return;
    }

    std::cout << "Available country codes:\n";
    for (const auto& code : countryCodes) {
        std::cout << code << "\n";
    }

    // Ask the user to select a country code
    std::string selectedCountryCode;
    std::cout << "Enter the country code to filter by: ";
    std::cin >> selectedCountryCode;

    // Get filtered candlestick data for the selected country
    auto filteredData = manager.getFilteredCandlestickDataByCountry(selectedCountryCode);

    if (filteredData.empty()) {
        std::cout << "No data available for the selected country: " << selectedCountryCode << "\n";
    } else {
        std::cout << "------ Filtered Candlestick Data for " << selectedCountryCode << " ------\n";
        for (const auto& candlestick : filteredData) {
            candlestick.print();
        }
    }
}