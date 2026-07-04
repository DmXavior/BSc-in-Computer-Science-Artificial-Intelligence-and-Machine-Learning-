/*
  ==============================================================================

    Track.h
    Created: 13 Feb 2025 8:42:38pm
    Author:  Devon Xavior

    ** All codes wrote without assistance **
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// Track Class
class Track
{
public:
    // Constructor
    Track(juce::File _file);

    // Public member variables
    juce::File file;        // Audio file
    juce::String title;     // File name (without extension)
    juce::URL url;          // URL derived from the file
    juce::String length;  // Track duration in "mm:ss" format
    
    // Operator Overloading
    bool operator==(const juce::String& other) const;
};
