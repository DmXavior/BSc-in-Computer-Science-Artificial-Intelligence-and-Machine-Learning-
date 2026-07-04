#ifndef PLOTDATA_H
#define PLOTDATA_H

#include <vector>          // For std::vector
#include "weatherManager.h" // For WeatherData type

// Function to plot the data
void plotData();

void plotAveragesAsBarGraph(const std::map<std::string, double>& averages);

void plotTemperatureTimeSeries(int startMonth, int endMonth, const std::vector<double>& predictedTemps);

#endif