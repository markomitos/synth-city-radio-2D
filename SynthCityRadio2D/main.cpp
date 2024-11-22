#define _CRT_SECURE_NO_WARNINGS
	
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "graphics_utils.h"
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "tinyfiledialogs.h"
#include <filesystem>
#include <thread>

#include "globals.h"
#include "music_utils.h"
namespace fs = std::filesystem;

int main(void)
{
    std::vector<std::string> musicFiles = chooseFiles();
    std::thread musicThread(playMusicFiles, musicFiles);

    GLFWwindow* window;
    unsigned int wWidth = 800;
    unsigned int wHeight = 800;
    const char wTitle[] = "Synth City Radio";
    int code = initGlfw(window, wWidth, wHeight, wTitle);
    if (code != 0) return code;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW failed to load!\n";
        return 3;
    }
    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");
    unsigned int VAO, VBO;

    float vertices[] =
    {   //X    Y      S    T 
        -1.0, 0.0,   0.0, 0.0,
        1.0, 0.0,   1.0, 0.0, 
        -1.0, 1.0,   0.0, 1.0,
        1.0, 1.0,   1.0, 1.0

    };
    unsigned int stride = (2 + 2) * sizeof(float);
	initVAO(&VAO, &VBO, vertices, sizeof(vertices), stride);
    unsigned int sunVAO, sunVBO;
    unsigned int sunShader = createShader("Shaders/sun.vert", "Shaders/sun.frag");
    std::vector<float> sunVertices = generateCircleVertices(0.0f, 0.26f, 0.25f, 100);
    stride = (2) * sizeof(float);
    init2cordVAO(&sunVAO, &sunVBO, sunVertices.data(), sunVertices.size() * sizeof(float), stride);
	unsigned backgroundTexture = loadTexture("Textures/city-background.png", unifiedShader);
    
    unsigned int horizontalVAO, horizontalVBO;
    std::vector<float> horizontalVertices = generateHorizontalLines(10);  
    initVAO(&horizontalVAO, &horizontalVBO, horizontalVertices.data(), horizontalVertices.size() * sizeof(float), 2 * sizeof(float));

    unsigned int verticalVAO, verticalVBO;
    std::vector<float> verticalVertices = generateVerticalLines(20);  
    init2cordVAO(&verticalVAO, &verticalVBO, verticalVertices.data(), verticalVertices.size() * sizeof(float), 2 * sizeof(float));

    unsigned int gridShader = createShader("Shaders/grid.vert", "Shaders/grid.frag");

    while (!glfwWindowShouldClose(window))
    {

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        drawBackground(backgroundTexture, unifiedShader, &VAO);
        drawSun(sunShader, &sunVAO, sunVertices.size() / 2);
        drawGrid(gridShader, &horizontalVAO, horizontalVertices.size() / 2, &verticalVAO, verticalVertices.size()/2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteTextures(1, &backgroundTexture);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(unifiedShader);
    glDeleteBuffers(1, &sunVBO);
    glDeleteVertexArrays(1, &sunVAO);
    glDeleteBuffers(1, &horizontalVBO);
    glDeleteVertexArrays(1, &horizontalVAO);
    glDeleteBuffers(1, &verticalVBO);
    glDeleteVertexArrays(1, &verticalVAO);
    glDeleteProgram(sunShader);
    glDeleteProgram(gridShader);

    musicThreadRunning = false;
    if (musicThread.joinable()) {
        musicThread.join();
    }

    glfwTerminate();
    return 0;

}