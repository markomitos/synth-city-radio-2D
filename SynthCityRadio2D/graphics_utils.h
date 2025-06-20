#pragma once
#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int initGlfw(GLFWwindow*& window, int width, int height, const char* title);
void initVAO(unsigned int* VAO, unsigned int* VBO, float vertices[], unsigned int size, unsigned int stride);
void init2cordVAO(unsigned int* VAO, unsigned int* VBO, float vertices[], unsigned int size, unsigned int stride);
void init2cordRGBVAO(unsigned int* VAO, unsigned int* VBO, float vertices[], unsigned int size, unsigned int stride);
void initTextVAO(unsigned int* VAO, unsigned int* VBO);
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
void drawVolumeControls(unsigned shader, unsigned* VAO);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void RenderText(unsigned shader, std::string text, float x, float y, float scale, glm::vec3 color, unsigned* VAO, unsigned* VBO);
int initFreeType();
void drawText(unsigned int shader, unsigned int* VAO, unsigned int* VBO);
void init3DTexturedVAO(unsigned int* VAO, unsigned int* VBO, const float* vertices, size_t size, size_t stride);
void drawCube(unsigned int shaderProgram, unsigned int* VAO, unsigned int numVertices, unsigned int textureID);
std::vector<float> generateHorizontalLines3D(int numLines);
std::vector<float> generateVerticalLines3D(int numLines);
void init3cordVAO(unsigned int* VAO, unsigned int* VBO, float vertices[], unsigned int sizeInBytes, unsigned int stride);
void drawGrid3D(unsigned int shader, unsigned int* horizontalVAO, unsigned int horizontalVertexCount, unsigned int* verticalVAO, unsigned int verticalVertexCount, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
std::vector<float> generateCircleVertices3D(float centerX, float centerY, float centreZ, float radius, int numSegments);
void drawSun3D(unsigned int shader, unsigned int* VAO, int size, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);

