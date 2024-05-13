/*
  ==============================================================================

    DJAudioPlayer.h
    Created: 12 Feb 2023 11:57:00am
    Author:  Benjamin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class DJAudioPlayer  : public AudioSource
{
public:

    // Constructor
    DJAudioPlayer(AudioFormatManager& _formatManager);

    // Destructor
    ~DJAudioPlayer();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void loadURL(URL audioURL);
    void setGain(double gain);
    void setSpeed(double ratio);
    void setPosition(double posInSecs);
    void setPositionRelative(double pos);

    void start();
    void stop();
    void pause();
    void loop();

    double getPositionRelative();
    double getTrackDuration();

    String getTrackName();
    String getTrackDurationMin();
    String getTrackDurationSec();

private:
    AudioFormatManager& formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    ResamplingAudioSource resampleSource{ &transportSource, false, 2 };
    File fileName;
};
