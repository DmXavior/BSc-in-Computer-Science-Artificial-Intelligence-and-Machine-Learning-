#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <functional>
#include "Track.h"
#include "DeckGUI.h"
#include "DJAudioPlayer.h"

//==============================================================================
// PlaylistComponent ** Own Codes **
class PlaylistComponent : public juce::Component,
                          public juce::TableListBoxModel,
                          public juce::Button::Listener,
                          public juce::TextEditor::Listener
{
public:
    // Constructor and Deconstructor ** Own Codes **
    PlaylistComponent(DeckGUI* _deckGUI1,
                      DeckGUI* _deckGUI2,
                      DJAudioPlayer* _playerForParsingMetaData);
    ~PlaylistComponent() override;
    
    // Component overrides
    void paint(juce::Graphics&) override;
    void resized() override;
    
    // TableListBoxModel overrides
    int getNumRows() override;
    void paintRowBackground(juce::Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    juce::Component* refreshComponentForCell(int rowNumber,
                                             int columnId,
                                             bool isRowSelected,
                                             Component* existingComponentToUpdate) override;
    
    // Event Listeners
    void buttonClicked(juce::Button* button) override;
    
private:
    // UI Components ** Own Codes **
    juce::TextButton importButton{ "Import Tracks" };
    juce::TextEditor searchField;
    juce::TableListBox library;
    juce::TextButton addToDeck1Button{ "Add to Deck 1" };
    juce::TextButton addToDeck2Button{ "Add to Deck 2" };
    
    juce::FileChooser fChooser{"Select a file..."};
    
    // Track Management ** Own Codes **
    std::vector<Track> tracks;
    DeckGUI* deckGUI1;
    DeckGUI* deckGUI2;
    DJAudioPlayer* playerForParsingMetaData;
    
    // UI Theme ** Own Codes **
    juce::Colour warmSand = juce::Colour::fromRGB(178, 132, 109);
    
    // Helper Functions ** Own Codes **
    juce::String getLength(juce::URL audioURL);
    juce::String secondsToMinutes(double seconds);
    
    void importTracks();
    void searchLibrary(juce::String searchText);
    void saveLibrary();
    void loadLibrary();
    void deleteFromTracks(int id);
    bool isInTracks(juce::String fileNameWithoutExtension);
    int whereInTracks(juce::String searchText);
    void loadInPlayer(DeckGUI* deckGUI);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistComponent)
};
