/*
  ==============================================================================

    WaveformDisplay.h
    Created: 12 Feb 2025 9:59:45pm
    Author:  Devon Xavior

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <string>

//==============================================================================
// WaveformDisplay Class
class WaveformDisplay  : public juce::Component,
                         public juce::ChangeListener
{
public:
    // Constructor and Deconstructor
    WaveformDisplay(juce::AudioFormatManager& formatManagerToUse,
                    juce::AudioThumbnailCache& cacheToUse);
    ~WaveformDisplay() override;
    
    // Component Lifecycle
    void paint (juce::Graphics&) override;

    // Event Handling
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    // Audio File Management
    void loadURL(juce::URL audioURL);
    
    // Playback and Looping ** Own Codes **
    void setPositionRelative(double pos);
    void setTrackLength(double newLength)
        {
            totalTrackLength = newLength;
            repaint();
        }
    
    void setLoopRegion(double start, double end)
    {
        loopStart = start;
        loopEnd = end;
        loopRegionSet = (end > start);
        repaint();
    }
    
    void clearLoopRegion()
    {
        loopRegionSet = false;
        repaint();
    }
    
private:
    // Internal State Variables
    juce::AudioThumbnail audioThumb;
    bool fileLoaded = false;
    double position = 0.0;
    std::string nowPlaying;
    
    // Track and Looping data ** Own Codes **
    double totalTrackLength = 0.0;
    double loopStart = 0.0;
    double loopEnd = 0.0;
    bool loopRegionSet = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
