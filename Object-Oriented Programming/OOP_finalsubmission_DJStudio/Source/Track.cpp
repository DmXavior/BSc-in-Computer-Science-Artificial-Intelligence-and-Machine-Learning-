/*
  ==============================================================================

    Track.cpp
    Created: 13 Feb 2025 8:42:38pm
    Author:  Devon Xavior
    
    ** All codes wrote without assistance **
  ==============================================================================
*/

#include "Track.h"

// Constructor
Track::Track(juce::File _file)
    : file(_file),
      title(_file.getFileNameWithoutExtension()),
      url(juce::URL{ _file })
{
    DBG("Created track with title: " << title);
}

// Operator Overloading
bool Track::operator==(const juce::String& other) const
{
    return title == other;
}
