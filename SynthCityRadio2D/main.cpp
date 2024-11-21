#define _CRT_SECURE_NO_WARNINGS
	
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "graphics_utils.h"

int main(void)
{
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
    std::vector<float> sunVertices = generateCircleVertices(0.0f, 0.26f, 0.25f, 500);
    stride = (2) * sizeof(float);
    initCircleVAO(&sunVAO, &sunVBO, sunVertices.data(), sunVertices.size() * sizeof(float), stride);
	unsigned backgroundTexture = loadTexture("Textures/city-background.png", unifiedShader);

    while (!glfwWindowShouldClose(window))
    {

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        glClearColor(0.5, 0.5, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        drawBackground(backgroundTexture, unifiedShader, &VAO);
        drawSun(sunShader, &sunVAO, sunVertices.size() / 2);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteTextures(1, &backgroundTexture);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(unifiedShader);
    glDeleteBuffers(1, &sunVBO);
    glDeleteVertexArrays(1, &sunVAO);
    glDeleteProgram(sunShader);

    glfwTerminate();
    return 0;

}