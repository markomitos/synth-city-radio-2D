#define _CRT_SECURE_NO_WARNINGS
	
#include <iostream>

#include <vector>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "graphics_utils.h"
#include "SFML/Audio.hpp"
#include <filesystem>
#include <thread>

#include "globals.h"
#include "music_utils.h"
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
namespace fs = std::filesystem;

int main(void)
{
    std::vector<std::string> musicFiles = chooseFiles();
    std::thread musicThread(playMusicFiles, musicFiles);

    GLFWwindow* window;
    unsigned int wWidth = 1000;
    unsigned int wHeight = 1000;
    const char wTitle[] = "Synth City Radio";

    int code = initGlfw(window, wWidth, wHeight, wTitle);
    if (code != 0) return code;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW failed to load!\n";
        return 3;
    }

    if (initFreeType()==-1)
    {
        std::cout << "FreeType failed to load!\n";
        return 4;
    }

    unsigned int textVAO, textVBO;
	initTextVAO(&textVAO,&textVBO);

    unsigned int stride = (2 + 2) * sizeof(float);
    unsigned int sunVAO, sunVBO;
    unsigned int sunShader = createShader("Shaders/sun.vert", "Shaders/sun.frag");
    std::vector<float> sunVertices = generateCircleVertices3D(0.0f, 0.01f, -0.99f, 0.2f, 100);
    stride = (2) * sizeof(float);
    init3cordVAO(&sunVAO, &sunVBO, sunVertices.data(), sunVertices.size() * sizeof(float), 3 * sizeof(float));

    unsigned int controlsVAO, controlsVBO;
    init2cordVAO(&controlsVAO, &controlsVBO, controlsVetices, sizeof(controlsVetices), stride);
    unsigned int controlsShader = createShader("Shaders/controls.vert", "Shaders/controls.frag");
    unsigned int textShader = createShader("Shaders/text.vert", "Shaders/text.frag");

    stride = (5) * sizeof(float);
    unsigned int volumeControlsVAO, volumeControlsVBO;
    init2cordRGBVAO(&volumeControlsVAO, &volumeControlsVBO, volumeControlsVetices, sizeof(volumeControlsVetices), stride);
    unsigned int volumeControlsShader = createShader("Shaders/volume.vert", "Shaders/volume.frag");

	glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    unsigned int cubeShader;
    unsigned int cubeTexture;
    unsigned int sideFacesVAO, sideFacesVBO;
    cubeShader = createShader("Shaders/cube.vert", "Shaders/cube.frag");
    init3DTexturedVAO(&sideFacesVAO, &sideFacesVBO, cubeVertices, sizeof(cubeVertices), 5 * sizeof(float));
    cubeTexture = loadTexture("Textures/city-background.png", cubeShader);

    unsigned int topFaceTexture;
    unsigned int topFaceVAO, topFaceVBO;
    init3DTexturedVAO(&topFaceVAO, &topFaceVBO, topFaceVertices, sizeof(topFaceVertices), 5 * sizeof(float));
    topFaceTexture = loadTexture("Textures/night-sky.jpg", cubeShader);

    unsigned int horizontalVAO, horizontalVBO;
    std::vector<float> horizontalVertices = generateHorizontalLines3D(30);
    init3cordVAO(&horizontalVAO, &horizontalVBO, horizontalVertices.data(), horizontalVertices.size() * sizeof(float), 3 * sizeof(float));

    unsigned int verticalVAO, verticalVBO;
    std::vector<float> verticalVertices = generateVerticalLines3D(30);
    init3cordVAO(&verticalVAO, &verticalVBO, verticalVertices.data(), verticalVertices.size() * sizeof(float), 3 * sizeof(float));

    unsigned int gridShader = createShader("Shaders/grid.vert", "Shaders/grid.frag");

    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float cameraYaw = -90.0f;
    float cameraPitch = 0.0f;
    float cameraSpeed = 0.03f;

    while (!glfwWindowShouldClose(window))
    {
        std::chrono::steady_clock::time_point frameStart = std::chrono::high_resolution_clock::now();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }else if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
            decrementVolume(5);
        }else if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
            incrementVolume(5);
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraYaw -= cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraYaw += cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPitch += cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPitch -= cameraSpeed;
        }

        if (cameraPitch > 89.0f) {
            cameraPitch = 89.0f;
        }
        if (cameraPitch < -89.0f) {
            cameraPitch = -89.0f;
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 front;
        front.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        front.y = sin(glm::radians(cameraPitch));
        front.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        cameraFront = glm::normalize(front);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f);

        glUseProgram(cubeShader);
        glUniformMatrix4fv(glGetUniformLocation(cubeShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(cubeShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(cubeShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        drawCube(cubeShader, &sideFacesVAO, sizeof(cubeVertices) / (5 * sizeof(float)), cubeTexture);
    	drawCube(cubeShader, &topFaceVAO, sizeof(topFaceVertices) / (5 * sizeof(float)), topFaceTexture);

        drawGrid3D(gridShader, &horizontalVAO, horizontalVertices.size() / 3, &verticalVAO, verticalVertices.size() / 3, model, view, projection);

        drawSun3D(sunShader, &sunVAO, sunVertices.size() / 3, model, view, projection);
        drawControls(controlsShader, &controlsVAO);
        drawVolumeControls(volumeControlsShader, &volumeControlsVAO);
        drawText(textShader, &textVAO, &textVBO);
        glfwSwapBuffers(window);
        glfwPollEvents();

        std::chrono::steady_clock::time_point frameEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = frameEnd - frameStart;

        float targetFrameTime = 1.0f / 60.0f;
        float elapsedTime = elapsed.count();

        if (elapsedTime < targetFrameTime)
        {
            std::this_thread::sleep_for(std::chrono::duration<float>(targetFrameTime - elapsedTime));
        }
    }

    glDeleteVertexArrays(1, &sideFacesVAO);
    glDeleteBuffers(1, &sideFacesVBO);
    glDeleteVertexArrays(1, &topFaceVAO);
    glDeleteBuffers(1, &topFaceVBO);
    glDeleteTextures(1, &cubeTexture);
    glDeleteTextures(1, &topFaceTexture);
	glDeleteProgram(cubeShader);
    glDeleteBuffers(1, &sunVBO);
    glDeleteVertexArrays(1, &sunVAO);
    glDeleteBuffers(1, &horizontalVBO);
    glDeleteVertexArrays(1, &horizontalVAO);
    glDeleteBuffers(1, &verticalVBO);
    glDeleteVertexArrays(1, &verticalVAO);
    glDeleteBuffers(1, &controlsVBO);
    glDeleteVertexArrays(1, &controlsVAO);
    glDeleteBuffers(1, &textVBO);
    glDeleteVertexArrays(1, &textVAO);
    glDeleteProgram(sunShader);
    glDeleteProgram(gridShader);
    glDeleteProgram(controlsShader);
    glDeleteProgram(textShader);

    musicThreadRunning = false;
    if (musicThread.joinable()) {
        musicThread.join();
    }

    glfwTerminate();
    return 0;

}