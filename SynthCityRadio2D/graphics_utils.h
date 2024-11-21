#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
int initGlfw(GLFWwindow*& window, int width, int height, const char* title);
void initVAO(unsigned int* VAO, unsigned int* VBO, float vertices[], unsigned int size, unsigned int stride);
void initCircleVAO(unsigned int* VAO, unsigned int* VBO, float vertices[], unsigned int size, unsigned int stride);
unsigned int createShader(const char* vsSource, const char* fsSource);
unsigned int compileShader(GLenum type, const char* source);
unsigned int loadImageToTexture(const char* filePath);
unsigned loadTexture(const char* filePath, unsigned shader);
void drawBackground(unsigned texture, unsigned shader, unsigned* VAO);
std::vector<float> generateCircleVertices(float centerX, float centerY, float radius, int numSegments);
void drawSun(unsigned int shader, unsigned int* VAO, int size);