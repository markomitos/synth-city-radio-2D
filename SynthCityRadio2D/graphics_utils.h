#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
int initGlfw(GLFWwindow*& window, int width, int height, const char* title);
void initVAO(unsigned int* VAO, unsigned int* VBO, float vertices[], unsigned int size, unsigned int stride);
void init2cordVAO(unsigned int* VAO, unsigned int* VBO, float vertices[], unsigned int size, unsigned int stride);
unsigned int createShader(const char* vsSource, const char* fsSource);
unsigned int compileShader(GLenum type, const char* source);
unsigned int loadImageToTexture(const char* filePath);
unsigned loadTexture(const char* filePath, unsigned shader);
void drawBackground(unsigned texture, unsigned shader, unsigned* VAO);
std::vector<float> generateCircleVertices(float centerX, float centerY, float radius, int numSegments);
void drawSun(unsigned int shader, unsigned int* VAO, int size);
std::vector<float> generateHorizontalLines(int numLines);
std::vector<float> generateVerticalLines(int numLines);
void drawGrid(unsigned int shader, unsigned int* horizontalVAO, unsigned int horizontalSize, unsigned int* verticalVAO, unsigned int verticalSize);
void drawControls(unsigned shader, unsigned* VAO);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);