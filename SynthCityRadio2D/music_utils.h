#pragma once
#include <string_view>
#include <vector>

std::vector<std::string> chooseFiles();
void playMusicFiles(std::vector<std::string> musicFiles);
void togglePlayPause();
void playNextSong();
void playPreviousSong();
void seekTo(float position);
float getCurrentSeek();