/*
  ==============================================================================

    DJAudioPlayer.cpp
    Created: 12 Feb 2025 9:59:26pm
    Author:  Devon Xavior

  ==============================================================================
*/

#include "DJAudioPlayer.h"

// Constructor and Destructor
DJAudioPlayer::DJAudioPlayer(juce::AudioFormatManager& _formatManager):formatManager(_formatManager)
{
}

DJAudioPlayer::~DJAudioPlayer()
{
}

// AudioSource Overrides
void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void DJAudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) // ** Own Codes **
{
    // Manual loop takes precedence over full-track looping
        if (manualLoopActive && transportSource.getCurrentPosition() >= loopEnd)
        {
            transportSource.setPosition(loopStart);
            if (!transportSource.isPlaying())
                transportSource.start();
            DBG("Manual loop restarted: repositioned to " << loopStart);
        }
        else if (isLooping && transportSource.getCurrentPosition() >= loopEnd)
        {
            transportSource.setPosition(loopStart);
            if (!transportSource.isPlaying())
                transportSource.start();
            DBG("Full track loop restarted.");
        }
        
        resampleSource.getNextAudioBlock(bufferToFill);
}

void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

// Audio Playback Controls ** Own Codes **
void DJAudioPlayer::loadFile(juce::URL audioURL)
{
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr)
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource (new juce::AudioFormatReaderSource (reader,
true));
        transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset (newSource.release());
    }
}

void DJAudioPlayer::setGain(double gain)
{
    if (gain < 0.0||gain > 1.0)
    {
        DBG("DJAudioPlayer::setGain gain should be between 0 and 1");
    }
    else {
        transportSource.setGain(gain);
    }
}

void DJAudioPlayer::setSpeed(double ratio)
{
    if (ratio <= 0 || ratio > 100.0)
    {
        DBG("DJAudioPlayer::setSpeed ratio should be between 0 and 100");
    }
    else {
        resampleSource.setResamplingRatio(ratio);
    }
}

void DJAudioPlayer::setPosition(double posInSecs)
{
    transportSource.setPosition(posInSecs);
}

void DJAudioPlayer::setPositionRelative(double pos)
{
    if (pos < 0.0 || pos > 1.0)
    {
        DBG("DJAudioPlayer::setPositionRelative pos should be between 0 and 1");
    }
    else {
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
}

double DJAudioPlayer::getPositionRelative()
{
    return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
}

double DJAudioPlayer::getLengthInSeconds()
{
    return transportSource.getLengthInSeconds();
}

double DJAudioPlayer::getCurrentPosition() // ** Own Codes **
{
    return transportSource.getCurrentPosition();
}

void DJAudioPlayer::start()
{
    if (transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds() - 0.01)
    {
        transportSource.setPosition(0.0);
    }
    transportSource.start();
}

void DJAudioPlayer::stop()
{
    transportSource.stop();
}

// Looping Controls ** Own Codes **
void DJAudioPlayer::setLooping(bool shouldLoop)
{
    if (shouldLoop)
    {
        loopStart = 0.0;
        loopEnd = transportSource.getLengthInSeconds();
        isLooping = true;
        DBG("Full track loop enabled: 0.0s to " << loopEnd << "s");
    }
    else
    {
        isLooping = false;
        DBG("Full track looping disabled.");
    }
}

void DJAudioPlayer::setManualLoop(double start, double end)
{
    double trackLength = transportSource.getLengthInSeconds();
    if (start >= 0.0 && end > start && end <= trackLength)
    {
        loopStart = start;
        loopEnd = end;
        manualLoopActive = true;
        // Optionally, disable full-track looping when manual loop is active.
        isLooping = false;
        DBG("Manual loop set: " << loopStart << "s to " << loopEnd << "s");
    }
    else
    {
        DBG("DJAudioPlayer::setManualLoop - Invalid loop range!");
    }
}

void DJAudioPlayer::clearManualLoop()
{
    manualLoopActive = false;
    // Optionally reset loop boundaries:
    loopStart = 0.0;
    loopEnd = transportSource.getLengthInSeconds();
    DBG("Manual loop cleared.");
}

void DJAudioPlayer::checkLooping()
{
    if (isLooping && transportSource.getCurrentPosition() >= loopEnd)
    {
        transportSource.setPosition(loopStart);
        transportSource.start(); // Ensure playback resumes
        DBG("Loop restarted (timer): repositioned to " << loopStart);
    }
}
