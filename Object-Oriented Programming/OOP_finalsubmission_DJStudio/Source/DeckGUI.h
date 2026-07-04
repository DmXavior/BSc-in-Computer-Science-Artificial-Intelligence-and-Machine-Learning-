/*
  ==============================================================================

    DeckGUI.h
    Created: 12 Feb 2025 9:59:11pm
    Author:  Devon Xavior

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"

//==============================================================================
// Custom LookAndFeel for Knobs ** Own Codes **
class CustomKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override
    {
        // Define knob properties
        auto radius = juce::jmin(width / 2, height / 2) - 10.0f;
        auto centerX = x + width * 0.5f;
        auto centerY = y + height * 0.45f;
        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        
        // Draw knob outline with a thicker stroke
        g.setColour(juce::Colour::fromRGB(178, 132, 109));
        g.drawEllipse(centerX - radius, centerY - radius, radius * 2, radius * 2, 6.0f);

        // Draw knob background
        g.setColour(juce::Colour::fromRGB(153, 204, 204));
        g.fillEllipse(centerX - radius + 1, centerY - radius + 1, (radius * 2) - 2, (radius * 2) - 2);

        // Draw knob marker (indicator line)
        g.setColour(juce::Colour::fromRGB(178, 132, 109));
        juce::Path marker;
        marker.startNewSubPath(centerX, centerY);
        marker.lineTo(centerX + radius * 0.9f * std::cos(angle), centerY + radius * 0.9f * std::sin(angle));
        g.strokePath(marker, juce::PathStrokeType(2.5f));
    }
    
    // Main slider layout
    juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override
        {
            juce::Slider::SliderLayout layout = LookAndFeel_V4::getSliderLayout(slider);

            layout.textBoxBounds.setY(layout.textBoxBounds.getY() - 8);

            return layout;
        }
    
    // Slider text box
    juce::Label* createSliderTextBox(juce::Slider& slider) override
        {
            auto* textBox = LookAndFeel_V4::createSliderTextBox(slider);
            textBox->setColour(juce::Label::textColourId, juce::Colours::darkgrey);
            return textBox;
        }
};

// DeckGUI Class
class DeckGUI  : public juce::Component,
                 public juce::Button::Listener,
                 public juce::Slider::Listener,
                 public juce::FileDragAndDropTarget,
                 public juce::Timer
{
public:
    // Constructor and Destructor
    DeckGUI(DJAudioPlayer* player,
            juce::AudioFormatManager & formatManagerToUse,
            juce::AudioThumbnailCache & cacheToUse);
    ~DeckGUI() override;

    // Component Lifecycle
    void paint (juce::Graphics&) override;
    void resized() override;

    // Event Listeners
    void buttonClicked(juce::Button*) override;
    void sliderValueChanged(juce::Slider*) override;
    
    // Drag-and-Drop support
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    
    // Timer Callback (for waveform updates)
    void timerCallback() override;

private:
    // Playback controls
    juce::TextButton playButton{ "PLAY" };
    juce::TextButton stopButton{ "STOP" };
    juce::TextButton loadButton{ "Select File" };
    
    // Looping controls ** Own Codes **
    juce::TextButton loopInButton { "LOOP IN" };
    juce::TextButton loopOutButton { "LOOP OUT" };
    juce::TextButton fullLoopToggleButton{ "Full Loop: Off" };
    juce::TextButton manualLoopToggleButton{ "Manual Loop: Off" };
    
    double loopStart = 0.0;
    double loopEnd = 0.0;
    bool fullLoopOn = false;
    bool manualLoopMode = false;
    
    // Sliders and Controls
    juce::Slider VolSlider;
    juce::Slider speedSlider;
    juce::Slider posSlider;
    
    // CustomLookAndFeel for speed slider knob ** Own Codes **
    CustomKnobLookAndFeel speedKnobLookAndFeel;
    
    // Custom colour
    juce::Colour warmSand = juce::Colour::fromRGB(178, 132, 109);
    
    // File Handling
    juce::FileChooser fChooser{"Select a file..."};
    
    // Labels
    juce::Label VolLabel;
    juce::Label speedLabel;
    
    // Utility and Processing
    void loadFile(juce::URL audioURL);

    DJAudioPlayer* player;
    
    //Waveform visual
    WaveformDisplay waveformDisplay;
    juce::SharedResourcePointer< juce::TooltipWindow > sharedTooltip; // ** Own Codes **

    friend class PlaylistComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
