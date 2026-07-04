/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 12 Feb 2025 9:59:45pm
    Author:  Devon Xavior

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveformDisplay.h"
#include "DJAudioPlayer.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(juce::AudioFormatManager& formatManagerToUse,
                                juce::AudioThumbnailCache& cacheToUse)
    : audioThumb(1000, formatManagerToUse, cacheToUse),
      fileLoaded(false),
      position(0)
{
    audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
}

// Component Rendering ** Own Codes **
void WaveformDisplay::paint (juce::Graphics& g)
{
    // Set the background colour to soft teal
    g.fillAll(juce::Colour::fromRGB(153, 204, 204));

    // Draw component border
    g.setColour(juce::Colour::fromRGB(178, 132, 109));
    g.drawRect(getLocalBounds(), 2);

    if (fileLoaded)
    {
        // Draw waveform in warm sand colour
        g.setColour(juce::Colour::fromRGB(178, 132, 109));
        audioThumb.drawChannel(g, getLocalBounds(), 0, audioThumb.getTotalLength(), 0, 1.0f);
        
        // Draw the position marker in white
        g.setColour(juce::Colours::white);
        g.fillRect(position * getWidth(), 0, 3, getHeight());

        // Display track name
        g.setColour(juce::Colours::floralwhite);
        g.setFont(16.0f);
        g.drawText(nowPlaying, getLocalBounds(), juce::Justification::centred, true);
    }
    else {
        // Draw placeholder text in white (if no track loaded)
        g.setColour(juce::Colours::white);
        g.setFont(25.0f);
        g.drawText("File not Loaded", getLocalBounds(), juce::Justification::centred, true);
    }
    
    // Draw loop region with overlay (if selected)
    if (loopRegionSet && totalTrackLength > 0)
    {
        // Calculate the x positions corresponding to loopStart and loopEnd
        int xStart = static_cast<int>((loopStart / totalTrackLength) * getWidth());
        int xEnd   = static_cast<int>((loopEnd   / totalTrackLength) * getWidth());
        int regionWidth = xEnd - xStart;

        g.setColour(juce::Colour::fromRGB(31, 13, 14).withAlpha(0.3f));
        g.fillRect(xStart, 0, regionWidth, getHeight());
    }
}

// Audio File Loading ** Own Codes **
void WaveformDisplay::loadURL(juce::URL audioURL)
{
    // Clear previous waveform data and loop region
    audioThumb.clear();
    clearLoopRegion();

    juce::File file = audioURL.getLocalFile();
    if (!file.existsAsFile())
    {
        DBG("WaveformDisplay::loadURL - File does not exist: " << file.getFullPathName());
        fileLoaded = false;
        repaint();
        return;
    }
    else
    {
        DBG("WaveformDisplay::loadURL - File exists: " << file.getFullPathName());
    }

    // Load track into waveform display
    fileLoaded = audioThumb.setSource(new juce::FileInputSource(file));

    if (fileLoaded)
    {
        DBG("WaveformDisplay::LOADED!");
        
        // Extract file name for display
        std::string justFile = audioURL.toString(false).toStdString();
        std::size_t startFilePos = justFile.find_last_of("/");
        std::size_t startExtPos = justFile.find_last_of(".");
        std::string fileName = justFile.substr(startFilePos + 1, startExtPos - startFilePos - 1);
        nowPlaying = fileName;

        // Use getTotalLength() to set the track length.
        totalTrackLength = audioThumb.getTotalLength();
        DBG("Total track length: " << totalTrackLength);
    }
    else
    {
        DBG("WaveformDisplay::NOT LOADED!");
        repaint();
    }
}

// Event Handling
void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    DBG("WaveformDisplay::CHANGE RECEIVED!");
    repaint();
}

// Playback Position and Looping ** Own Codes **
void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position && pos==pos)
    {
        position = pos;
        repaint();
    }
}
