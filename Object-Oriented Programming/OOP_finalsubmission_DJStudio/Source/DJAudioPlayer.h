/*
  ==============================================================================

    DJAudioPlayer.h
    Created: 12 Feb 2025 9:59:26pm
    Author:  Devon Xavior

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// DJ AuddioPlayer Class
class DJAudioPlayer : public juce::AudioSource
{
    public:
        // Constructor and Destructor
        DJAudioPlayer(juce::AudioFormatManager& _formatManager);
        ~DJAudioPlayer() override;
    
        // AudioSource Overrides
        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
        void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
        void releaseResources() override;
        
        // Audio Playback Controls
        void loadFile(juce::URL audioURL);
        void setGain(double gain);
        void setSpeed(double ratio);
        void setPosition(double posInSecs);
        void setPositionRelative(double pos);

        void start();
        void stop();
    
        // Looping Functionality ** Own Codes **
        void setLooping(bool shouldLoop);
        void checkLooping();
        void setManualLoop(double start, double end);
        void clearManualLoop();
        
        // Utility Functions ** Own Codes **
        double getPositionRelative();
        double getLengthInSeconds();
        double getCurrentPosition();

    private:
        // Audio Processing Components
        juce::AudioFormatManager& formatManager;
        std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
        juce::AudioTransportSource transportSource;
        juce::ResamplingAudioSource resampleSource{&transportSource, false, 2};
        
        // Looping Variables ** Own Codes **
        bool isLooping = false;
        bool manualLoopActive = false;
        double loopStart = 0.0;
        double loopEnd = 0.0;
};
