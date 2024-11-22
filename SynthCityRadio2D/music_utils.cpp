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

std::vector<std::string> chooseFiles()
{
    const char* folderPath = tinyfd_selectFolderDialog("Select a Folder", NULL);
    std::vector<std::string> musicFiles;
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

void playMusicFiles(std::vector<std::string> musicFiles)
{
    std::cout << "Found " << musicFiles.size() << " music files." << std::endl;

    sf::Music music;
    for (const auto& file : musicFiles) {
        std::cout << "Playing: " << file << std::endl;

        if (!music.openFromFile(file)) {
            std::cerr << "Failed to load " << file << std::endl;
            continue;
        }
        music.play();

        while (musicThreadRunning) {

            sf::sleep(sf::seconds(0.1f));
        }

    }

}