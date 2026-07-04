#ifndef WEATHERMANAGER_H
#define WEATHERMANAGER_H

#include <vector>
#include <string>
#include <set>
#include <map>
#include "candlestickData.h"

struct WeatherData {
    std::string timestamp;           // e.g. "1980-01-01 01:00"
    std::vector<double> temperatures;
};

// weatherManger class
class weatherManager {
private:
    std::vector<WeatherData> weatherData;    // Raw data loaded from the CSV
    std::vector<Candlestick> candlestickData; // Computed yearly candlestick data
    std::string dataFilePath;                // File path for the loaded data

public:
    // Loads the CSV file and stores it in weatherData
    void loadData(const std::string& filePath);

    // Returns a reference to the raw loaded data
    const std::vector<WeatherData>& getWeatherData() const;

    // Displays a preview (first 'n' rows) of the raw loaded data
    void showPreview(size_t rows = 5) const;

    // Clears all loaded data and candlestick data
    void clearData() {
        weatherData.clear();
        candlestickData.clear();
        dataFilePath.clear();
    }

    // Computes candlestick data aggregated by year, with optional country filtering
    void computeCandlestickData(const std::string& selectedCountryCode = "global");

    // Retrieves the available country codes from the data
    std::set<std::string> getCountryCodes() const;

    // Filters RAW weather data by a specific country code
    std::vector<WeatherData> getRawWeatherDataByCountry(const std::string& countryCode) const;

    // Filters RAW weather data by a specific date range
    std::vector<WeatherData> getRawWeatherDataByDateRange(const std::string& startDate, const std::string& endDate) const;
    
    // Access the computed candlestick data
    const std::vector<Candlestick>& getCandlestickData() const;

    // FILTERED candlestick data by a specific country code
    std::vector<Candlestick> getFilteredCandlestickDataByCountry(const std::string& countryCode) const;

    // FILTERED candlestick data for a specific date range
    std::vector<Candlestick> getFilteredCandlestickDataByDateRange(int startYear, int endYear) const;

    // Helper function for computing averages
    std::map<std::string, double> computeAverages(
        const std::string& countryCode, const std::string& startDate, const std::string& endDate, const std::string& granularity) const;

    // Computes daily averages for a given date range
    std::map<std::string, double> computeDailyAverages(
        const std::string& countryCode, const std::string& startDate, const std::string& endDate) const;

    // Computes monthly averages for a given date range
    std::map<std::string, double> computeMonthlyAverages(
        const std::string& countryCode, const std::string& startDate, const std::string& endDate) const;

    // Computes yearly averages for a given date range
    std::map<std::string, double> computeYearlyAverages(
        const std::string& countryCode, const std::string& startDate, const std::string& endDate) const;
};

#endif // WEATHERMANAGER_H