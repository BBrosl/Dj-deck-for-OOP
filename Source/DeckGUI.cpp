/*
  ==============================================================================

    DeckGUI.cpp
    Created: 12 Feb 2023 11:25:17am
    Author:  Benjamin

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player, 
    AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse, 
    String _deckID) : 
    player(_player), 
    waveformDisplay(formatManagerToUse, cacheToUse),
    deckID(_deckID)
{
    //addAndMakeVisible(deckNumber);
    addAndMakeVisible(trackName);
    addAndMakeVisible(trackDuration);

    //buttons on deck
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(repeatButton);

    //sliders on deck
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);

    //setting slider style and slider text
    volSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    speedSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    speedSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    volSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);

    //labels
    addAndMakeVisible(volumeLabel);
    addAndMakeVisible(speedLabel);
    addAndMakeVisible(positionLabel);
    //deck numbers
    addAndMakeVisible(deckNumber);
   
    //showing waveform
    addAndMakeVisible(waveformDisplay);

    //styles
    getLookAndFeel().setColour(Slider::thumbColourId, Colours::orangered);
    getLookAndFeel().setColour(Slider::rotarySliderFillColourId, Colours::ghostwhite);
    //pointers to buttons
    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    //pointers to sliders
    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);

    //range of sliders
    volSlider.setRange(0.0, 1.0);
    speedSlider.setRange(0.1, 5.0);
    posSlider.setRange(0.0, 1.0);

    //presetting values
    volSlider.setValue(1.0);
    speedSlider.setValue(1.0);
    posSlider.setValue(0.0);

    posSlider.setNumDecimalPlacesToDisplay(2);
    volSlider.setNumDecimalPlacesToDisplay(2);
    speedSlider.setNumDecimalPlacesToDisplay(2);

    //adding text to decks
    deckNumber.setText("Deck " + deckID, dontSendNotification);
    trackName.setText("Name : No File", dontSendNotification);
    trackDuration.setText("Duration : No File", dontSendNotification);

    //adding text to sliders
    volumeLabel.setText("Volume", dontSendNotification);
    speedLabel.setText("Speed", dontSendNotification);
    positionLabel.setText("POS", dontSendNotification);

    volumeLabel.setJustificationType(Justification::centredBottom);
    speedLabel.setJustificationType(Justification::centredBottom);
    
    //positioning top labels
    deckNumber.setJustificationType(Justification::centred);
    trackName.setJustificationType(Justification::centred);
    trackDuration.setJustificationType(Justification::centred);

    volumeLabel.attachToComponent(&volSlider, false);
    speedLabel.attachToComponent(&speedSlider, false);
    positionLabel.attachToComponent(&posSlider, true);

    startTimer(100);

    isPlaying = false;
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint (juce::Graphics& g)
{
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.fillAll(Colours::salmon);
    g.setColour (juce::Colours::lightpink);
    g.drawRect (getLocalBounds(), 1);   // draw outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);

}

void DeckGUI::resized()
{
    auto labelposition = 50;
    double rowH = getHeight() / 9.0;
    double posWidth = getWidth() / 3;
    //track details
    trackName.setBounds(0, 0, posWidth, rowH);
    trackDuration.setBounds(posWidth, 0, posWidth, rowH);
    repeatButton.setBounds(getWidth() - posWidth, 0, posWidth, rowH);
    //buttons
    playButton.setBounds(0, rowH , posWidth, rowH);
    stopButton.setBounds(posWidth, rowH, posWidth, rowH);
    loadButton.setBounds(posWidth * 2, rowH, posWidth, rowH);

    deckNumber.setBounds(posWidth, rowH*2, posWidth, rowH * 3);
    //sliders
    volSlider.setBounds(0, rowH * 3 - 13 , getWidth() / 2, rowH * 3);
    speedSlider.setBounds(getWidth() / 2, rowH * 3 - 13 , getWidth() / 2, rowH * 3);
    posSlider.setBounds(labelposition, rowH * 6 + 5, getWidth()  - labelposition, rowH * 0.75);
    //waveform display
    waveformDisplay.setBounds(0, rowH * 7, getWidth(), rowH * 2);
}

void DeckGUI::buttonClicked(Button* button)
{
    //DBG(" MainComponent::buttonClicked: They clicked a button");
    if (button == &playButton)
    {
        DBG(" MainComponent::buttonClicked: playButton");

        if (player->getTrackDuration() > 0)
        {
            if (isPlaying)
            {
                player-> pause();
                isPlaying = false;
                //if track not plying change to a play button
                button->setButtonText("Play");
            }
            else {
                player->start();
                isPlaying = true;
                //if track playing change button to a pause button
                button->setButtonText("Pause");
            }
        }
       
    }

    if (button == &stopButton)
    {
        DBG(" MainComponent::buttonClicked: stopButton");
        //set back to default
        waveformDisplay.setPositionRelative(0);
        posSlider.setValue(0);
        player->stop();
        isPlaying = false;
        playButton.setButtonText("Play");
    }

    if (button == &loadButton)
    {

        FileChooser chooser("Select a Wave file to play...");
        if (chooser.browseForFileToOpen())
        {
            //URL audioURL = URL{ chooser.getResult() };
            player->loadURL(URL{ chooser.getResult() });
            waveformDisplay.loadURL(URL{ chooser.getResult() });
            //adding track info
            trackName.setText("Track Name: " + player -> getTrackName(), dontSendNotification);
            trackDuration.setText("Track Duration: " + player -> getTrackDurationMin() + " min " 
                                  + player -> getTrackDurationSec() + " sec", dontSendNotification);
        }
    }
}

void DeckGUI::sliderValueChanged(Slider* slider)
{
    if (slider == &volSlider)
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

bool DeckGUI::isInterestedInFileDrag(const StringArray& files)
{
    DBG("DeckGUI::isInterestedInFileDrag") ;
    return true;
}

void DeckGUI::filesDropped(const StringArray& files, int x, int y)
{
    DBG("DeckGUI::filesDropped" );
    if (files.size() == 1)
    {
        player->loadURL(URL{ File{files[0]} });

        //loads waveform upon drag and dropping files into GUI
        waveformDisplay.loadURL(URL{ File{files[0]} });

        trackName.setText("Track Name: " + player->getTrackName(), dontSendNotification);
        trackDuration.setText("Track Duration: " + player->getTrackDurationMin() + " min "
            + player->getTrackDurationSec() + " sec", dontSendNotification);
    }
    
}

void DeckGUI::timerCallback()
{
    waveformDisplay.setPositionRelative(player->getPositionRelative());
    posSlider.setValue(player->getPositionRelative());

    if (player->getPositionRelative() > 1)
    {    //toggle logic
        if (repeatButton.getToggleState())
        {   
            //set position slider back to start
            posSlider.setValue(0);
            player->loop();
            isPlaying = true;
        }

        else
        {
            posSlider.setValue(0);
            player->stop();
            isPlaying = false;
            playButton.setButtonText("Play");
        }
    }
}

void DeckGUI::sendURLToload(URL url)
{
    playButton.setButtonText("Play");
    player->loadURL(url);
    isPlaying = false;

    // load audio to display waveform
    waveformDisplay.loadURL(url);

    // setting name and duration after loading
    trackName.setText("Track Name: " + player-> getTrackName() ,dontSendNotification);
    trackDuration.setText("Track Duration: " + player->getTrackDurationMin() + " min "
        + player->getTrackDurationSec() + " sec", dontSendNotification);
}
