#ifndef PREDICTDATA_H
#define PREDICTDATA_H

#include <vector>
#include <string>

// Predict temperature difference between two months for a specific year
double predictTemperatureDifference(
    const std::string& countryCode,
    int startMonth,
    int endMonth,
    int predictionYear);

// Function to get predicted temperatures for the specified months and year
std::vector<double> getPredictedTemperatures(const std::string& countryCode, int startMonth, int endMonth, int predictionYear);

#endif // PREDICTDATA_H