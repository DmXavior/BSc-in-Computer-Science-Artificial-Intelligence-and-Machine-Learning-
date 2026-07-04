#ifndef CANDLESTICKDATA_H
#define CANDLESTICKDATA_H

#include <string>
#include <iostream>

// Candlestick Class
class Candlestick {
private:
    std::string date; // could be "YYYY", "YYYY-MM", or full "YYYY-MM-DD
    double open; // average mean temperature of the previous time frame
    double high; // highest temperature in the current time frame
    double low; // lowest temperature in the current time frame
    double close; // average mean temperature of the current time frame

public:
    // Constructor to initialize candlestick data
    Candlestick(const std::string& date, double open, double high, double low, double close)
        : date(date), open(open), high(high), low(low), close(close) {}

    // Getters
    std::string getDate()  const { return date; }
    double      getOpen()  const { return open; }
    double      getHigh()  const { return high; }
    double      getLow()   const { return low; }
    double      getClose() const { return close; }

    // For debugging or text-based display
    void print() const {
        std::cout << "Date: "   << date 
                  << ", Open: " << open
                  << ", High: " << high
                  << ", Low: "  << low
                  << ", Close: "<< close << std::endl;
    }
};

#endif // CANDLESTICKDATA_H