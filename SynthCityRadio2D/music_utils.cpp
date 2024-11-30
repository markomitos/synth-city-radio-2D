#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "tinyfiledialogs.h"
#include "music_utils.h"
#include <filesystem>
#include <thread>
#include <SFML/Audio.hpp>

#include "globals.h"

namespace fs = std::filesystem;
sf::Music music;
size_t currentSongIndex = 0;
std::vector<std::string> musicFiles;

std::vector<std::string> chooseFiles()
{
    const char* folderPath = tinyfd_selectFolderDialog("Select a Folder", NULL);
    
    if (!folderPath) {
        std::cout << "No folder selected." << std::endl;
        return musicFiles;
    }

    std::cout << "Selected folder: " << folderPath << std::endl;

   
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            auto extension = entry.path().extension().string();
            if (extension == ".ogg" || extension == ".wav" || extension == ".flac") {
                musicFiles.push_back(entry.path().string());
            }
        }
    }
    return musicFiles;
}

void changeSongDisplay(std::string file)
{
    int startIndex = file.find_last_of("\\") + 1;
    int endIndex = file.find_last_of(".");
    if (startIndex >= 0) currentSong = file.substr(startIndex, endIndex - startIndex);
    std::string totalDurationMinutes = std::to_string(static_cast<int>(music.getDuration().asSeconds()) / 60);
    if (totalDurationMinutes.size() < 2) totalDurationMinutes = "0" + totalDurationMinutes;
    std::string totalDurationSeconds = std::to_string(static_cast<int>(music.getDuration().asSeconds()) % 60);
    if (totalDurationSeconds.size() < 2) totalDurationSeconds = "0" + totalDurationSeconds;
    std::string currentDurationMinutes = std::to_string(static_cast<int>(music.getPlayingOffset().asSeconds()) / 60);
    if (currentDurationMinutes.size() < 2) currentDurationMinutes = "0" + currentDurationMinutes;
    std::string currentDurationSeconds = std::to_string(static_cast<int>(music.getPlayingOffset().asSeconds()) % 60);
    if (currentDurationSeconds.size() < 2) currentDurationSeconds = "0" + currentDurationSeconds;
	totalDuration = totalDurationMinutes + ":" + totalDurationSeconds;
    currentDuration = currentDurationMinutes + ":" + currentDurationSeconds;
}

void changeCurrentOffset()
{
    std::string currentDurationMinutes = std::to_string(static_cast<int>(music.getPlayingOffset().asSeconds()) / 60);
    if (currentDurationMinutes.size() < 2) currentDurationMinutes = "0" + currentDurationMinutes;
    std::string currentDurationSeconds = std::to_string(static_cast<int>(music.getPlayingOffset().asSeconds()) % 60);
    if (currentDurationSeconds.size() < 2) currentDurationSeconds = "0" + currentDurationSeconds;
    currentDuration = currentDurationMinutes + ":" + currentDurationSeconds;
}

void playMusicFiles(std::vector<std::string> musicFiles)
{
    std::cout << "Found " << musicFiles.size() << " music files." << std::endl;

    for (const auto& file : musicFiles) {
        std::cout << "Playing: " << file << std::endl;

        if (!music.openFromFile(file)) {
            std::cerr << "Failed to load " << file << std::endl;
            continue;
        }
        music.play();
        changeSongDisplay(file);
        while (musicThreadRunning) {

            sf::sleep(sf::seconds(0.1f));
        }

    }

}

void togglePlayPause() {
    if (isMusicPlaying) {
        music.pause();  
        isMusicPlaying = false;  
        std::cout << "Music paused." << std::endl;
    }
    else {
        music.play();  
        isMusicPlaying = true;  
        std::cout << "Music playing." << std::endl;
    }
}

void playNextSong() {
    if (music.getStatus() == sf::Music::Playing) {
        music.stop();
    }

    currentSongIndex = (currentSongIndex + 1) % musicFiles.size();

    if (!music.openFromFile(musicFiles[currentSongIndex])) {
        std::cerr << "Failed to load " << musicFiles[currentSongIndex] << std::endl;
        return;
    }

    std::cout << "Playing next song: " << musicFiles[currentSongIndex] << std::endl;
    music.play();
    isMusicPlaying = true;
    changeSongDisplay(musicFiles[currentSongIndex]);
}

void playPreviousSong() {
    if (music.getStatus() == sf::Music::Playing) {
        music.stop();  
    }

    if (currentSongIndex == 0) {
        currentSongIndex = musicFiles.size() - 1;
    }
    else {
        currentSongIndex--;
    }

    if (!music.openFromFile(musicFiles[currentSongIndex])) {
        std::cerr << "Failed to load " << musicFiles[currentSongIndex] << std::endl;
        return;
    }

    std::cout << "Playing previous song: " << musicFiles[currentSongIndex] << std::endl;
    music.play();
    isMusicPlaying = true;
    changeSongDisplay(musicFiles[currentSongIndex]);
}

void seekTo(float position) {
    if (music.getStatus() == sf::Music::Playing || music.getStatus() == sf::Music::Paused) {
        sf::Time duration = music.getDuration();
        sf::Time seekPosition = duration * position;
        music.setPlayingOffset(seekPosition);
    }
}

float getCurrentSeek()
{
    float currentSeek = music.getPlayingOffset().asSeconds();
	float totalDuration = music.getDuration().asSeconds();

	float normalizedSeek = currentSeek / totalDuration;
    changeCurrentOffset();
	return std::clamp(normalizedSeek, 0.0f, 1.0f);

}

float getCurrentVolume()
{
    return music.getVolume();
}

void setCurrentVolume(float volume)
{
    music.setVolume(volume);
}

void decrementVolume(float decrement)
{
    music.setVolume(std::clamp(music.getVolume() - decrement,0.0f, 100.0f));
}

void incrementVolume(float increment)
{
    music.setVolume(std::clamp(music.getVolume() + increment, 0.0f, 100.0f));
}