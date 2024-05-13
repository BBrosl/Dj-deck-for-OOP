/*
  ==============================================================================

    DJAudioPlayer.cpp
    Created: 12 Feb 2023 11:57:00am
    Author:  Benjamin

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DJAudioPlayer.h"

//==============================================================================
DJAudioPlayer::DJAudioPlayer(AudioFormatManager& _formatManager)
    : formatManager(_formatManager)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

DJAudioPlayer::~DJAudioPlayer()
{

}

void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void DJAudioPlayer::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    resampleSource.getNextAudioBlock(bufferToFill);
}
void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

void DJAudioPlayer::loadURL(URL audioURL)
{
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr) // good file!
    {
        std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader,
            true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
    }

    fileName = File(audioURL.toString(false));
}
void DJAudioPlayer::setGain(double gain)
{
    if (gain < 0 || gain > 1.0)
    {
        DBG("DJAudioPlayer::setGain gain should be between 0 and 1");
    }
    else {
        transportSource.setGain(gain);
    }

}
void DJAudioPlayer::setSpeed(double ratio)
{
    if (ratio < 0 || ratio > 100.0)
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
    if (pos < 0 || pos > 1.0)
    {
        DBG("DJAudioPlayer::setPositionRelative pos should be between 0 and 1");
    }
    else {
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
}

void DJAudioPlayer::start()
{
    transportSource.start();
}
void DJAudioPlayer::stop()
{
    transportSource.stop();
}
void DJAudioPlayer::pause()
{
    transportSource.stop();
}
void DJAudioPlayer::loop()
{
    setPosition(0);
    transportSource.start();
}

double DJAudioPlayer::getPositionRelative()
{
    if (transportSource.getCurrentPosition() == 0)
    {
        return 0;
    }
    else
    {
        return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
    }
}

//obtaining duration of track
double DJAudioPlayer::getTrackDuration()
{
    return transportSource.getLengthInSeconds();
}

//strings used for printing values onto deck
String DJAudioPlayer::getTrackName()
{
    return fileName.getFileNameWithoutExtension();
}

String DJAudioPlayer::getTrackDurationMin()
{
    int trackMin = int(floor(double(transportSource.getLengthInSeconds() / 60)));
    return std::to_string(trackMin);
}

String DJAudioPlayer::getTrackDurationSec()
{
    int trackSec = int(floor(double(int(transportSource.getLengthInSeconds()) % 60)));
    return std::to_string(trackSec);
}