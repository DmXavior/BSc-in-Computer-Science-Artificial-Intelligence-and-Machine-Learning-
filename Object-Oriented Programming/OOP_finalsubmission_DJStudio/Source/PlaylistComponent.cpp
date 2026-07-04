/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 12 Feb 2025 10:00:01pm
    Author:  Devon Xavior

    ** Most codes wrote without assistance **
  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"
#include "Track.h"

//Constructor and Deconstructor ** Own Codes **
PlaylistComponent::PlaylistComponent (DeckGUI* _deckGUI1,
                                      DeckGUI* _deckGUI2,
                                      DJAudioPlayer* _playerForParsingMetaData)
    : deckGUI1(_deckGUI1),
      deckGUI2(_deckGUI2),
      playerForParsingMetaData(_playerForParsingMetaData)
{
    // Add UI components ** Own Codes **
    addAndMakeVisible(importButton);
    addAndMakeVisible(searchField);
    addAndMakeVisible(library);
    addAndMakeVisible(addToDeck1Button);
    addAndMakeVisible(addToDeck2Button);
    
    // Set up listeners ** Own Codes **
    importButton.addListener(this);
    searchField.addListener(this);
    addToDeck1Button.addListener(this);
    addToDeck2Button.addListener(this);
    
    // Configure search field appearance and placeholder text ** Own Codes **
    searchField.setTextToShowWhenEmpty("Type track title then press ENTER to search Playlist", juce::Colours::grey);
    searchField.onReturnKey = [this] { searchLibrary(searchField.getText()); };
    
    // Set up table columns ** Own Codes **
    library.setModel(this);
    library.getHeader().addColumn("Title", 1, 200);
    library.getHeader().addColumn("Duration", 2, 100);
    library.getHeader().addColumn("Actions", 3, 100);
    
    // Load any saved library from disk
    loadLibrary();
}

PlaylistComponent::~PlaylistComponent()
{
    saveLibrary();
}

// UI Rendering ** Own Codes **
void PlaylistComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(195, 221, 218));
    
    // Set button colours
    addToDeck1Button.setColour(juce::TextButton::buttonColourId, warmSand);
    addToDeck2Button.setColour(juce::TextButton::buttonColourId, warmSand);
    importButton.setColour(juce::TextButton::buttonColourId, warmSand);
}

void PlaylistComponent::resized()
{
    auto area = getLocalBounds();
    int controlHeight = 40;
    
    // Layout top controls
    importButton.setBounds(area.removeFromTop(controlHeight).reduced(5));
    searchField.setBounds(area.removeFromTop(controlHeight).reduced(5));
    
    // Allocate most space to the table
    library.setBounds(area.removeFromTop(area.getHeight() - controlHeight * 2).reduced(5));
    
    // Layout bottom buttons
    auto bottomArea = area.reduced(5);
    addToDeck1Button.setBounds(bottomArea.removeFromLeft(bottomArea.getWidth() / 2).reduced(2));
    addToDeck2Button.setBounds(bottomArea.reduced(2));
}

// Table Handling ** Own Codes **
int PlaylistComponent::getNumRows()
{
    return static_cast<int>(tracks.size());
}

void PlaylistComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    g.fillAll(rowIsSelected ? juce::Colours::lightblue : juce::Colours::white);
}

void PlaylistComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (rowNumber < getNumRows())
    {
        if (columnId == 1)
        {
            g.drawText(tracks[rowNumber].title, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
        }
        if (columnId == 2)
        {
            g.drawText(tracks[rowNumber].length, 2, 0, width - 4, height, juce::Justification::centred, true);
        }
    }
}

juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate)
{
    if (columnId == 3)
    {
        if (existingComponentToUpdate == nullptr)
        {
            auto* btn = new juce::TextButton{ "REMOVE" };
            btn->setComponentID(std::to_string(rowNumber));
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    return existingComponentToUpdate;
}

//Button Handling ** Own Codes **
void PlaylistComponent::buttonClicked(juce::Button* button)
{
    if (button == &importButton)
        {
            DBG("Import button clicked");
            importTracks();
            library.updateContent();
        }
        else if (button == &addToDeck1Button)
        {
            DBG("Add to Deck 1 clicked");
            loadInPlayer(deckGUI1);
        }
        else if (button == &addToDeck2Button)
        {
            DBG("Add to Deck 2 clicked");
            loadInPlayer(deckGUI2);
        }
        else
        {
            int id = std::stoi(button->getComponentID().toStdString());
            DBG(tracks[id].title + " removed from Library");
            deleteFromTracks(id);
            library.updateContent();
        }
}
// Track Management ** Own Codes **
void PlaylistComponent::loadInPlayer(DeckGUI* deckGUI)
{
    int selectedRow{ library.getSelectedRow() };
    if (selectedRow != -1)
    {
        DBG("Adding: " << tracks[selectedRow].title << " to Player");
        deckGUI->loadFile(tracks[selectedRow].url);
    }
    else
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::InfoIcon,
            "Add on to Deck Info:",
            "Select track to add to deck",
            "OK",
            nullptr
        );
    }
}

void PlaylistComponent::importTracks()
{
    DBG("Loading tracks from file...");

    //initialize file chooser
    auto fileChooserFlags = juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultipleItems;
    
    fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& fc)
        {
            auto files = fc.getResults(); // Get selected files
            for (const auto& file : files)
            {
                juce::String fileNameWithoutExtension = file.getFileNameWithoutExtension();
                if (!isInTracks(fileNameWithoutExtension)) // If not already loaded
                {
                    Track newTrack{ file };
                    juce::URL audioURL{ file };
                    newTrack.length = getLength(audioURL);
                    tracks.push_back(newTrack);
                    DBG("loaded file: " << newTrack.title);
                    library.updateContent();
                }
                else // Display info message
                {
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::InfoIcon,
                        "Load information:",
                        fileNameWithoutExtension + " already loaded",
                        "OK"
                    );
                }
            }
        }
    );
}

bool PlaylistComponent::isInTracks(juce::String fileNameWithoutExtension)
{
    return (std::find(tracks.begin(), tracks.end(), fileNameWithoutExtension) != tracks.end());
}

void PlaylistComponent::deleteFromTracks(int id)
{
    tracks.erase(tracks.begin() + id);
}

// Track Length Conversion ** Own Codes **
juce::String PlaylistComponent::getLength(juce::URL audioURL)
{
    playerForParsingMetaData->loadFile(audioURL);
    return secondsToMinutes(playerForParsingMetaData->getLengthInSeconds());
}

juce::String PlaylistComponent::secondsToMinutes(double seconds)
{
    int secondsRounded = static_cast<int>(std::round(seconds));
    return juce::String(secondsRounded / 60) + ":" + juce::String(secondsRounded % 60).paddedLeft('0', 2);
}

// Library Management ** Own Codes **
void PlaylistComponent::searchLibrary(juce::String searchText)
{
    DBG("Searching library for: " << searchText);
    if (searchText != "")
    {
        int rowNumber = whereInTracks(searchText);
        library.selectRow(rowNumber);
    }
    else
    {
        library.deselectAllRows();
    }
}

int PlaylistComponent::whereInTracks(juce::String searchText)
{
    auto it = std::find_if(tracks.begin(), tracks.end(),
                               [&searchText](const Track& obj) { return obj.title.contains(searchText); });

    return (it != tracks.end()) ? std::distance(tracks.begin(), it) : -1;
}

void PlaylistComponent::saveLibrary()
{
    // create .csv to save library
    std::ofstream myLibrary("my-library.csv");

    // save library to file
    for (Track& t : tracks)
    {
        myLibrary << t.file.getFullPathName() << "," << t.length << "\n";
    }
}

void PlaylistComponent::loadLibrary()
{
    std::ifstream myLibrary("my-library.csv");
        std::string filePath, length;

    while (getline(myLibrary, filePath, ','))
    {
        Track newTrack{ juce::File{ filePath } };
        getline(myLibrary, length);
        newTrack.length = length;
        tracks.push_back(newTrack);
    }
}
