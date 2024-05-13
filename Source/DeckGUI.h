/*
  ==============================================================================

    DeckGUI.h
    Created: 12 Feb 2023 11:25:17am
    Author:  Benjamin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
//==============================================================================
/*
*/
class DeckGUI  : public juce::Component,
                 public Button::Listener,
                 public Slider::Listener,
                 public FileDragAndDropTarget,
                 public Timer
{
public:
    DeckGUI(DJAudioPlayer* player, AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& cacheToUse, 
        String deckID);
    ~DeckGUI() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    /** implement Button::Listener */
    void buttonClicked(Button*) override;

    /** implement Slider::Listener */
    void sliderValueChanged(Slider* slider) override;

    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;

    void timerCallback() override;

    void sendURLToload(URL);//send url of audio

private:
    //deck buttons
    TextButton playButton{ "Play" };
    TextButton stopButton{ "Stop" };
    TextButton loadButton{ "Load" };

    //toggles
    ToggleButton repeatButton{ "Repeat" };

    //deck sliders
    Slider volSlider;
    Slider speedSlider;
    Slider posSlider;

    //titles
    Label deckNumber;
    Label trackName;
    Label trackDuration;

    Label volumeLabel;
    Label speedLabel;
    Label positionLabel;

    String deckID;

    bool isPlaying;

    DJAudioPlayer* player;

    WaveformDisplay waveformDisplay;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
