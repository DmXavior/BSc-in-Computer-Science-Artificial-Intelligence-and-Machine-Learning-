#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Setting size of main component
    setSize (800, 600);

    // Request audio recording permissions if required
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Set up 2 audio channels (no input, 2 outputs)
        setAudioChannels (0, 2);
    }
    
    // Add child components to main UI
    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);
    addAndMakeVisible(playlistComponent);

    // Register audio formats
    formatManager.registerBasicFormats();
}
//==============================================================================
// Destructor
MainComponent::~MainComponent()
{
    // Shut down the audio device and clears the audio sources
    shutdownAudio();
}

//==============================================================================
// Audio Processing Methods
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // Audio Players
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // Mixer Source
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Pass audio to mixer source
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // Release resources for players and mixer
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
}

//==============================================================================
// UI Rendering methods ** Layout are own codes **
void MainComponent::paint (juce::Graphics& g)
{
    // Fill background with Soft Muted Teal (Light Aqua)
    g.fillAll(juce::Colour::fromRGB(224, 242, 241));
}

void MainComponent::resized()
{
    // Layout playlist component in top half of component
    playlistComponent.setBounds(0, 0, getWidth(), getHeight() / 2);
    
    // Layout deck GUIs in the bottom half.
    int halfWidth = getWidth() / 2;
    int halfHeight = getHeight() / 2;
        
    deckGUI1.setBounds(0, halfHeight, halfWidth, halfHeight);
    deckGUI2.setBounds(halfWidth, halfHeight, halfWidth, halfHeight);
}
