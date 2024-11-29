#pragma once
#include <atomic>
#include <map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
inline std::atomic<bool> musicThreadRunning(true);
inline std::atomic<bool> isMusicPlaying(true);

inline float vertices[] =
{   //X    Y      S    T 
    -1.0, 0.0,   0.0, 0.0,
    1.0, 0.0,   1.0, 0.0,
    -1.0, 1.0,   0.0, 1.0,
    1.0, 1.0,   1.0, 1.0

};

inline float controlsVetices[] = {
    //back
    -0.25, -0.4,
    -0.25, -0.5,
    -0.225, -0.4,
    -0.225, -0.5,
    -0.225, -0.45,
    -0.15, -0.4,
    -0.15, -0.5,

    //pause
    -0.04, -0.4,
    -0.04, -0.5,
    -0.015, -0.4,
    -0.015, -0.5,

	0.04, -0.4,
    0.04, -0.5,
    0.015, -0.4,
    0.015, -0.5,

    //play
    -0.04, -0.4,
    -0.04, -0.5,
    0.04, -0.45,

    //forward
	0.15, -0.4,
    0.15, -0.5,
    0.225, -0.45,
    0.225, -0.5,
    0.225, -0.4,
    0.25, -0.5,
    0.25, -0.4,

    //slider
    -0.4, -0.6,
    -0.4, -0.61,
    0.4, -0.6,
    0.4, -0.61,

    //slider knob
    -0.41, -0.62,
    -0.41, -0.59,
    -0.39, -0.62,
    -0.39, -0.59,

};

inline std::string currentSong = "No song playing";
inline std::string currentDuration = "0:00";
inline std::string totalDuration = "0:00";

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};
inline std::map<char, Character> Characters;