/*
  ==============================================================================

    PlaylistComponent.h
    Created: 12 Feb 2023 11:57:43am
    Author:  Benjamin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DeckGUI.h"
#include <vector>
#include <string>
//==============================================================================
/*
*/
class PlaylistComponent  : public juce::Component,
                           public TableListBoxModel,
                           public Button::Listener,
                           public FileDragAndDropTarget,
                           public Timer
{
public:
    PlaylistComponent(AudioFormatManager& _formatManager, DeckGUI* deckGUI1, DeckGUI* deckGUI2);
    ~PlaylistComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    //tablelistbox model implementation
    int getNumRows() override;

    void paintRowBackground(Graphics&,
        int rowNumber,
        int width,
        int height,
        bool rowIsSelected) override;

    void paintCell(Graphics&,
        int rowNumber,
        int columnId,
        int width,
        int height,
        bool rowIsSelected) override;

    Component* refreshComponentForCell(int rowNumber,
        int columnId,
        bool isRowSelected,
        Component* existingComponentToUpdate) override;

    void buttonClicked(Button* button) override;

    //for drag and drop
    void filesDropped(const StringArray& files, int x, int y) override;
    bool isInterestedInFileDrag(const StringArray& files) override;

    void timerCallback() override;

private:
    TableListBox tableComponent;

    //check audio file
    void processTrack(File&);
    void createTxtFile();

    //playlist title
    Label PlaylistTrackTitle;
    //buttons
    TextButton loadToPlaylist{ "Load tracks" };
    TextButton searchButton{ "Search" };
    TextButton clearButton{ "Clear" };

    TextEditor searchBar;

    //metadata information
    std::vector<std::string> trackTitles;
    std::vector<std::string> trackDurations;
    std::vector<URL> trackPath;

    //temp information
    std::vector<std::string> tempTitle;
    std::vector<std::string> tempDuration;
    std::vector<URL> tempTrackPath;

    AudioFormatManager& formatManager;
    
    //decks
    DeckGUI* deckGUI1;
    DeckGUI* deckGUI2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};
