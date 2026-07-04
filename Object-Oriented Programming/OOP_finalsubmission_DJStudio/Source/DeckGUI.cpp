/*
  ==============================================================================

    DeckGUI.cpp
    Created: 12 Feb 2025 9:59:11pm
    Author:  Devon Xavior

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DeckGUI.h"

//==============================================================================
// Constructor
DeckGUI::DeckGUI(DJAudioPlayer* _player,
                 juce::AudioFormatManager& formatManagerToUse,
                 juce::AudioThumbnailCache& cacheToUse) : player(_player),
                                                          waveformDisplay(formatManagerToUse, cacheToUse)
{
    // Add and make visible UI components
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    
    // Loop Buttons ** Own Codes **
    addAndMakeVisible(fullLoopToggleButton);
    addAndMakeVisible(manualLoopToggleButton);
    addAndMakeVisible(loopInButton);
    addAndMakeVisible(loopOutButton);
    
    addAndMakeVisible(loadButton);
    addAndMakeVisible(VolSlider);
    addAndMakeVisible(VolLabel); // ** Own Codes **
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(speedLabel); // ** Own Codes **
    addAndMakeVisible(posSlider);
    addAndMakeVisible(waveformDisplay);

    // Add button listeners
    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    
    // ** Own Codes **
    fullLoopToggleButton.addListener(this);
    manualLoopToggleButton.addListener(this);
    loopInButton.addListener(this);
    loopOutButton.addListener(this);
    
    // Add slider listeners
    VolSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);
    
    // Position slider colors ** Own Codes **
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(178, 132, 109));
    getLookAndFeel().setColour(juce::Slider::trackColourId, juce::Colour::fromRGB(195, 221, 218));
    
    // Position Slider Configuration ** Own Codes **
    posSlider.setRange(0.0, 1.0);
    posSlider.setNumDecimalPlacesToDisplay(2);
    posSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    posSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
    // Volume Slider Configuration ** Own Codes **
    VolSlider.setRange(0.0, 1.0);
    VolSlider.setNumDecimalPlacesToDisplay(2);
    VolSlider.setValue(0.5); //default volume half of max vol
    VolSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    VolSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
    // Volume Label Configuration ** Own Codes **
    VolLabel.setText("Volume", juce::dontSendNotification);
    VolLabel.attachToComponent(&VolSlider, false);
    VolLabel.setJustificationType(juce::Justification::centred);
    VolLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    
    // Speed Label Configuration ** Own Codes **
    speedLabel.setText("Speed", juce::dontSendNotification);
    speedLabel.attachToComponent(&speedSlider, false);
    speedLabel.setJustificationType(juce::Justification::centred);
    speedLabel.setColour(juce::Label::textColourId, juce::Colours::black);
   
    // Speed Knob Configuration ** Own Codes **
    speedSlider.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    speedSlider.setRange(0.5, 3.0, 0.01);  // Speed range from 0.5x (slow) to 3x (fast)
    speedSlider.setValue(1.0);  // Default speed at 1.0x (normal speed)
    speedSlider.setMouseDragSensitivity(150);
    speedSlider.setLookAndFeel(&speedKnobLookAndFeel);

    startTimer(500);
}

// Destructor
DeckGUI::~DeckGUI()
{
    stopTimer();
}

// UI Rendering ** Own Codes **
void DeckGUI::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB(224, 242, 241)); // Soft Muted Teal background (Light Aqua)

    g.setColour (juce::Colour::fromRGB(178, 132, 109)); // Warm Sand outline (Soft Copper)
    g.drawRect (getLocalBounds(), 2);   // Slightly thicker outline for a refined look
    
    loadButton.setColour(juce::TextButton::buttonColourId, warmSand);
    playButton.setColour(juce::TextButton::buttonColourId, warmSand);
    stopButton.setColour(juce::TextButton::buttonColourId, warmSand);
    
}

// Button Click Events ** Own Codes **
void DeckGUI::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        std::cout << "Play button was clicked " << std::endl;
        player->start();
    }
    if (button == &stopButton)
    {
        std::cout << "Stop button was clicked " << std::endl;
        player->stop();

    }
    if (button == &fullLoopToggleButton)
    {
        // Toggle full-track loop mode.
        fullLoopOn = !fullLoopOn;
        player->setLooping(fullLoopOn); // This method loops the entire track.
        fullLoopToggleButton.setButtonText(fullLoopOn ? "Full Loop: On" : "Full Loop: Off");
                
        // When full-loop is enabled, disable manual loop mode.
        if (fullLoopOn)
        {
            manualLoopMode = false;
            manualLoopToggleButton.setButtonText("Manual Loop: Off");
        }
    }
    if (button == &manualLoopToggleButton)
    {
        manualLoopMode = !manualLoopMode;
        manualLoopToggleButton.setButtonText(manualLoopMode ? "Manual Loop: On" : "Manual Loop: Off");
            
        if (!manualLoopMode)
        {
            // When turning off manual loop mode, clear the manual loop in the player.
            player->clearManualLoop();
            waveformDisplay.clearLoopRegion();
        }
        else
        {
            // When manual loop is enabled, disable full-track looping if it's active.
            fullLoopOn = false;
            fullLoopToggleButton.setButtonText("Full Loop: Off");
            // Optionally, clear any previously set manual loop.
            player->clearManualLoop();
        }
    }
    if (button == &loopInButton)
    {
        if (manualLoopMode)
        {
            double pos = player->getCurrentPosition();
            loopStart = pos;
            DBG("Loop In set at: " << loopStart << " seconds");
            // Optionally update the waveform display to show only the start until out is set:
            waveformDisplay.setLoopRegion(loopStart, loopEnd); // loopEnd may be 0 if not set yet
        }
        else
        {
            DBG("Manual loop mode is off. Enable it to set loop points.");
        }
    }
    if (button == &loopOutButton)
    {
        if (manualLoopMode)
        {
            double pos = player->getCurrentPosition();
            loopEnd = pos;
            if (loopEnd > loopStart)
            {
                player->setManualLoop(loopStart, loopEnd);
                DBG("Loop Out set at: " << loopEnd << ". Manual loop active.");
                waveformDisplay.setLoopRegion(loopStart, loopEnd);
            }
            else
            {
                DBG("Invalid manual loop: Loop Out must be after Loop In.");
            }
        }
        else
        {
            DBG("Manual loop mode is off. Enable it to set loop points.");
        }
    }
    if (button == &loadButton)
    {
        auto fileChooserFlags =
        juce::FileBrowserComponent::canSelectFiles;
        fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
        {
            player->loadFile(juce::URL{chooser.getResult()});
            // and now the waveformDisplay as well
            waveformDisplay.loadURL(juce::URL{chooser.getResult()});
            
            double length = player->getLengthInSeconds();
            waveformDisplay.setTrackLength(length);
        });
    }
}

// Slider Value Changed Events
void DeckGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &VolSlider)
    {
        player->setGain(slider->getValue());
    }
    if (slider == &speedSlider)
    {
        player->setSpeed(slider->getValue());
    }
    if (slider == &posSlider)
    {
        player->setPositionRelative(slider->getValue());
    }
}

// File Drag and Drop Events
bool DeckGUI::isInterestedInFileDrag(const juce::StringArray &files)
{
    DBG("DeckGUI::isInterestedInFileDrag");
    return true;
}

void DeckGUI::filesDropped(const juce::StringArray &files, int x, int y) // ** Own Codes **
{
    DBG("DeckGUI::filesDropped");
    if (files.size() == 1)
    {
        juce::File droppedFile(files[0]);
        juce::URL fileURL(droppedFile);
        player->loadFile(fileURL);
        waveformDisplay.loadURL(fileURL);
    }
}

// File Load Function
void DeckGUI::loadFile(juce::URL audioURL)
{
    DBG("DeckGUI::loadFile called");
    player->loadFile(audioURL);
    waveformDisplay.loadURL(audioURL);
}

// Component Resizing ** Own Codes **
void DeckGUI::resized()
{
    double rowH = getHeight() / 6;
    double colW = getWidth() / 4;
    
    waveformDisplay.setBounds(0, 0, getWidth(), rowH * 2);
    
    posSlider.setBounds(0, rowH * 2, getWidth(), rowH);
    VolSlider.setBounds(colW * 2 - 20, rowH * 3 + 10, 60, rowH * 2 - 10);
    speedSlider.setBounds(colW * 3 - 30, rowH * 3 + 10, 80, rowH * 2 - 10);

    playButton.setBounds(colW/8 + 10, rowH * 5 + 10, colW - 10 , rowH - 20);
    stopButton.setBounds(colW + 30 , rowH * 5 + 10, colW - 10, rowH - 20);
    loadButton.setBounds(colW*2 + 40 , rowH * 5 + 10, colW + 10, rowH - 20);
    
    fullLoopToggleButton.setBounds(colW - 60, rowH * 3 - 5, 100, 20);
    manualLoopToggleButton.setBounds(colW - 60, rowH * 3 + 20, 100, 20);
    loopInButton.setBounds(colW - 65, rowH * 3 + 55, 50, 40);
    loopOutButton.setBounds(colW - 5, rowH * 3 + 55, 50, 40);
}

// Timer Callback (Waveform Position Update) ** Own Codes **
void DeckGUI::timerCallback()
{
    player->checkLooping();
    
    double relPos = player->getPositionRelative();
    // Check for NaN or non-finite values
    if (std::isnan(relPos) || !std::isfinite(relPos))
    {
        relPos = 0.0;
    }

    relPos = juce::jlimit(0.0, 1.0, relPos);

    if (relPos > 0.9999)
        relPos = 1.0;
    else
        relPos = std::round(relPos * 1000.0) / 1000.0;
    
    waveformDisplay.setPositionRelative(relPos);
    posSlider.setValue(relPos, juce::dontSendNotification);
}
