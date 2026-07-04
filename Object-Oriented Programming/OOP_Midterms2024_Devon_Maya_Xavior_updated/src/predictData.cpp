#include "predictData.h"
#include "weatherManager.h"
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>

extern weatherManager manager; // Access global weatherManager instance

// Helper function to calculate historical monthly averages
std::vector<double> calculateHistoricalMonthlyAverages(const std::string& countryCode, int month) {
    const auto& weatherData = manager.getRawWeatherDataByCountry(countryCode);
    std::vector<double> monthlyAverages;

    // Loop through historical data
    for (int year = 1980; year <= 2019; ++year) {
        std::vector<double> temperatures;

        // Extract temperatures for the specified month and year
        for (const auto& data : weatherData) {
            if (std::stoi(data.timestamp.substr(0, 4)) == year &&
                std::stoi(data.timestamp.substr(5, 2)) == month) {
                temperatures.push_back(data.temperatures.front());
            }
        }

        if (!temperatures.empty()) {
            double average = std::accumulate(temperatures.begin(), temperatures.end(), 0.0) / temperatures.size();
            monthlyAverages.push_back(average);
        }
    }

    return monthlyAverages;
}

// Linear regression prediction for a single month
double predictMonthlyTemperature(const std::vector<double>& historicalAverages, int predictionYear) {
    int startYear = 1980;
    int endYear = 2019;

    // Prepare x (years) and y (average temperatures)
    std::vector<int> years;
    for (int i = startYear; i <= endYear; ++i) {
        years.push_back(i);
    }

    // Linear regression calculations
    int n = years.size();
    double sumX = std::accumulate(years.begin(), years.end(), 0.0);
    double sumY = std::accumulate(historicalAverages.begin(), historicalAverages.end(), 0.0);
    double sumXY = 0.0;
    double sumX2 = 0.0;

    for (int i = 0; i < n; ++i) {
        sumXY += years[i] * historicalAverages[i];
        sumX2 += years[i] * years[i];
    }

    double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    double intercept = (sumY - slope * sumX) / n;

    // Predict temperature for the given prediction year
    return slope * predictionYear + intercept;
}

// Predict temperature difference between two months
double predictTemperatureDifference(
    const std::string& countryCode,
    int startMonth,
    int endMonth,
    int predictionYear) {
    // Calculate historical averages for the two months
    auto startMonthAverages = calculateHistoricalMonthlyAverages(countryCode, startMonth);
    auto endMonthAverages = calculateHistoricalMonthlyAverages(countryCode, endMonth);

    if (startMonthAverages.empty() || endMonthAverages.empty()) {
        std::cerr << "Error: Insufficient historical data for prediction.\n";
        return 0.0;
    }

    // Predict temperatures for the specified year
    double startMonthPrediction = predictMonthlyTemperature(startMonthAverages, predictionYear);
    double endMonthPrediction = predictMonthlyTemperature(endMonthAverages, predictionYear);

    // Calculate and return the difference
    return endMonthPrediction - startMonthPrediction;
}

// Function to get predicted temperatures for the specified months and year
std::vector<double> getPredictedTemperatures(const std::string& countryCode, int startMonth, int endMonth, int predictionYear) {
    std::vector<double> predictedTemps;

    // Loop over the months in the specified range
    for (int month = startMonth; month <= endMonth; ++month) {
        // Calculate the historical monthly averages for the specified month
        auto monthlyAverages = calculateHistoricalMonthlyAverages(countryCode, month);

        // Use the linear regression model to predict the temperature for the given month and year
        double predictedTemp = predictMonthlyTemperature(monthlyAverages, predictionYear);
        
        predictedTemps.push_back(predictedTemp);  // Store the predicted temperature for this month
    }

    return predictedTemps;  // Return the list of predicted temperatures
}

