/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 12 Feb 2023 11:57:43am
    Author:  Benjamin

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"
#include <algorithm>

//==============================================================================
PlaylistComponent::PlaylistComponent(AudioFormatManager& _formatManager, DeckGUI* _deckGUI1, DeckGUI* _deckGUI2) :
                                     formatManager(_formatManager), deckGUI1(_deckGUI1), deckGUI2(_deckGUI2)
{
    //columns
    tableComponent.getHeader().addColumn("Track title", 1, 400);
    tableComponent.getHeader().addColumn("Duration", 2, 100);
    tableComponent.getHeader().addColumn("Load to deck 1", 3, 100);
    tableComponent.getHeader().addColumn("Load to deck 2", 4, 100);
    tableComponent.getHeader().addColumn("remove", 5, 50);

    tableComponent.setModel(this);

    //making buttons show
    addAndMakeVisible(PlaylistTrackTitle);
    addAndMakeVisible(loadToPlaylist);
    addAndMakeVisible(searchButton);
    addAndMakeVisible(clearButton);
    addAndMakeVisible(searchBar);
    addAndMakeVisible(tableComponent);

    //colours
    tableComponent.setColour(ListBox::backgroundColourId, Colours::salmon);
    loadToPlaylist.setColour(TextButton::ColourIds::buttonColourId,Colours::hotpink);
    PlaylistTrackTitle.setColour(Label::backgroundColourId, Colours::darkorange);

    //pointer to listensers
    loadToPlaylist.addListener(this);
    searchButton.addListener(this);
    clearButton.addListener(this);

    PlaylistTrackTitle.setText("Deck Playlist", dontSendNotification);
    PlaylistTrackTitle.setJustificationType(Justification::centred);
    //update
    startTimer(100);

    // reading text file
    File audioFile = File::getCurrentWorkingDirectory().getChildFile("audioPlaylist.txt");

    std::unique_ptr<FileInputStream> input(audioFile.createInputStream());

        if (input->openedOk()) // if stream can be open
        {
            int lineNum = 0; // accessing the vectors

            while (input->isExhausted() != true) // making sure there is data
            {
                std::string line = input->readNextLine().toStdString();
                int index = lineNum % 3;

                if (index == 0) // access track names
                {
                    tempTitle.push_back(line);
                }

                else if (index == 1) // access track durationss
                {
                    tempDuration.push_back(line);
                }

                else if (index == 2) // access track path
                {
                    tempTrackPath.push_back(URL{ line });
                }

                lineNum++;

                // clearing original vectors
                trackTitles.clear();
                trackDurations.clear();
                trackPath.clear();

                // inserting information into vectors
                trackTitles = tempTitle;
                trackDurations = tempDuration;
                trackPath = tempTrackPath;

                // updating the tabletable
                tableComponent.updateContent();
            }
        }
        else {
            //output if file not present
            DBG("text file does not exist");
        }
    
}

PlaylistComponent::~PlaylistComponent()
{
    stopTimer();
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    //clearing background
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    
}

void PlaylistComponent::resized()
{   
    double rowH = getHeight() / 6;
    double searchBarWidth = getWidth() / 6;
    
    //position of components
    PlaylistTrackTitle.setBounds(0, 0, getWidth(), rowH);

    searchBar.setBounds(0, rowH, searchBarWidth * 4, rowH);
    searchButton.setBounds(searchBarWidth * 4, rowH, searchBarWidth, rowH);
    clearButton.setBounds(searchBarWidth * 5, rowH, searchBarWidth, rowH);

    loadToPlaylist.setBounds(0, rowH*2, getWidth(), rowH);

    tableComponent.setBounds(0, rowH * 3, getWidth(), rowH * 3);
}

int PlaylistComponent::getNumRows()
{
    return trackTitles.size();
}

void PlaylistComponent::paintRowBackground(Graphics& g,
    int rowNumber,
    int width,
    int height,
    bool rowIsSelected)
{
    if (rowNumber < trackTitles.size())
    {
        if (rowIsSelected)
        {
            //fill when selected
            g.fillAll(Colours::orange);
        }
        else {
            g.fillAll(Colours::salmon);
        }
    }
}

void PlaylistComponent::paintCell(Graphics& g,
    int rowNumber,
    int columnId,
    int width,
    int height,
    bool rowIsSelected)
{
    if (rowNumber < trackTitles.size())
    {
        if (rowIsSelected)
        {
            g.setColour(Colours::black); //color changes when selected
        }
        else {
            g.setColour(Colours::white); //color when unselected
        }

        if (columnId == 1)//track titles
        {
            g.drawText(trackTitles[rowNumber], 2, 0, width, height, Justification::centredLeft, true);
        }

        else if (columnId == 2) //track Duration
        {
            g.drawText(trackDurations[rowNumber], 2, 0, width, height, Justification::centredLeft, true);
        }
    }
}

Component* PlaylistComponent::refreshComponentForCell(int rowNumber,
    int columnId,
    bool isRowSelected,
    Component* existingComponentToUpdate)
{

    if (columnId == 3 || columnId == 4 || columnId == 5)
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = nullptr;
            String id{ std::to_string(rowNumber) };

            //making buttons for columns based on id
            if (columnId == 3)
            {
                btn = new TextButton{ "Load deck 1" };
            }
            else if (columnId == 4){
                btn = new TextButton{ "Load deck 2" };
            }
            else if (columnId == 5) {
                btn = new TextButton{ "Delete" };
            }

            //check for button then update and add listener
            if (btn)
            {
                btn->setComponentID(id);
                btn->addListener(this);
                existingComponentToUpdate = btn;

                //setting colours to buttons
                btn->setColour(TextButton::buttonColourId, Colours::deepskyblue);

            }

        }
    }

    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(Button* button)
{
    //button functions
    if (button == &loadToPlaylist)
    {
        FileChooser chooser{ "Select Tracks.." };
        if (chooser.browseForMultipleFilesToOpen())
        {
            for (const auto& file : chooser.getResults())
                processTrack(File{ file });
            tableComponent.updateContent();
        }
    }
    else if (button == &searchButton)
    {
        //clearing information
        trackTitles.clear();
        trackDurations.clear();
        trackPath.clear();

        if (searchBar.getText().length() > 0)
        {
            for (int i = 0; i < tempTitle.size(); ++i)
            {
                std::string trackTitleFound = tempTitle[i];
                std::transform(trackTitleFound.begin(), trackTitleFound.end(), trackTitleFound.begin(), ::tolower);

                if (trackTitleFound.find(searchBar.getText().toLowerCase().toStdString()) != std::string::npos)
                {
                    trackTitles.push_back(tempTitle[i]);
                    trackDurations.push_back(tempDuration[i]);
                    trackPath.push_back(tempTrackPath[i]);
                }
            }
        }
        else
        {
            //updating vectors
            trackTitles = tempTitle;
            trackDurations = tempDuration;
            trackPath = tempTrackPath;
        }

        searchBar.setText("");
        searchBar.setTextToShowWhenEmpty("Search for track...", Colours::deeppink);
        searchBar.clear();
        tableComponent.updateContent();
    }
    else if (button == &clearButton)
    {
        trackTitles = tempTitle;
        trackDurations = tempDuration;
        trackPath = tempTrackPath;

        searchBar.setText("");
        searchBar.setTextToShowWhenEmpty("Search for track...", Colours::deeppink);
        searchBar.clear();
        tableComponent.updateContent();
    }
    else if (button != &loadToPlaylist && button != &searchButton && button != &clearButton)
    {
        const auto& buttonType = button->getButtonText().toStdString();
        const auto buttonID = std::stoi(button->getComponentID().toStdString());

        if (buttonType == "Load deck 1")
            deckGUI1->sendURLToload(trackPath[buttonID]);
        else if (buttonType == "Load deck 2")
            deckGUI2->sendURLToload(trackPath[buttonID]);
        else if (buttonType == "Delete")
        {
            //Erase from both track and temp vectors
            trackTitles.erase(trackTitles.begin() + buttonID);
            trackDurations.erase(trackDurations.begin() + buttonID);
            trackPath.erase(trackPath.begin() + buttonID);
            tempTitle.erase(tempTitle.begin() + buttonID);
            tempDuration.erase(tempDuration.begin() + buttonID);
            tempTrackPath.erase(tempTrackPath.begin() + buttonID);

            //update table and create empty file
            tableComponent.updateContent();
            createTxtFile();
        }
    }
}


bool PlaylistComponent::isInterestedInFileDrag(const StringArray& files)
{
    return true;
}

void PlaylistComponent::filesDropped(const StringArray& files, int x, int y)
{
    //std::cout << "file dropped" << std::endl;
    DBG(" File dropped");

    for (int i = 0; i < files.size(); ++i)
    {
        //processing for playlist
        processTrack(File{ files[i] });
        //update table
        tableComponent.updateContent();
    }
}

void PlaylistComponent::timerCallback()
{
}

void PlaylistComponent::processTrack(File& file)
{
    //create reader for the audio file
    auto* reader = (formatManager.createReaderFor(file));

    //create audio source from the reader
    std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));

    //calculate track duration
    int durationOfTrack = reader->lengthInSamples / reader->sampleRate;
    //concatanating duration to get desired format
    std::string durationMinSec = std::to_string(durationOfTrack / 60) + " mins " + std::to_string(durationOfTrack % 60) + " secs";

    //get the filename
    std::string filename = file.getFileNameWithoutExtension().toStdString();

    //add the track to the main and temporary vectors
    trackTitles.push_back(filename);
    tempTitle.push_back(filename);

    trackDurations.push_back(durationMinSec);
    tempDuration.push_back(durationMinSec);

    trackPath.push_back(URL{ file });    
    tempTrackPath.push_back(URL{ file });

    //save the audio playlist into txt file
    File audioFile = File::getCurrentWorkingDirectory().getChildFile("audioPlaylist.txt");

    //check if file exists
    if (!audioFile.existsAsFile())
    {
        //if file does not exist, create it
        DBG("txt file does not exist");
        auto newCreateFile = audioFile.create();
        if (!newCreateFile.wasOk())
        {
            DBG("An error occured");
            return;
        }
    }

    //adding track information to the text file
    File::getCurrentWorkingDirectory().getChildFile("audioPlaylist.txt").appendText(filename + "\n");
    File::getCurrentWorkingDirectory().getChildFile("audioPlaylist.txt").appendText(durationMinSec + "\n");
    File::getCurrentWorkingDirectory().getChildFile("audioPlaylist.txt").appendText(URL{ file }.toString(false).toStdString() + "\n");
}

void PlaylistComponent::createTxtFile()
{
    // deleting existing file
    File::getCurrentWorkingDirectory().getChildFile("audioPlaylist.txt").deleteFile();
    // saving list of audios into a txt file
    File audioFile = File::getCurrentWorkingDirectory().getChildFile("audioPlaylist.txt");

    if (!audioFile.existsAsFile()) // txt file does not exist
    {
        // file has been deleted
        auto newCreateFile = audioFile.create();

        if (!newCreateFile.wasOk()) // if txt file cannot be created
        {
            return;
        }
    }

    for (int i = 0; i < trackTitles.size(); ++i)
    {
        // editing the file with information of the track
        File::getCurrentWorkingDirectory().getChildFile("audioPlaylist.txt")
            .appendText(trackTitles[i] + "\n");
        File::getCurrentWorkingDirectory().getChildFile("audioPlaylist.txt")
            .appendText(trackDurations[i] + "\n");
        File::getCurrentWorkingDirectory().getChildFile("audioPlaylist.txt")
            .appendText(trackPath[i].toString(false).toStdString() + "\n");
    }
}

