#include <fstream>
#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <math.h>
#include "music_utils.h"
#include "globals.h"

bool isDragging = false;
float knobPosition = 0.0f;

int initGlfw(GLFWwindow*& window, int width, int height, const char* title)
{
    if (!glfwInit())
    {
        std::cout << "GLFW library failed to load!\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Window failed to load!\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);

    return 0;
}

void initVAO(unsigned int* VAO, unsigned int* VBO, float vertices[], unsigned int size, unsigned int stride)
{
    glGenVertexArrays(1, VAO);
    glBindVertexArray(*VAO);
    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void init2cordVAO(unsigned int* VAO, unsigned int* VBO, float vertices[], unsigned int size, unsigned int stride)
{
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);

    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "File successfully read from path \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Error reading file from path \"" << source << "\"!" << std::endl;
    }
    std::string temp = ss.str();
    const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);

    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}

unsigned int createShader(const char* vsSource, const char* fsSource)
{

    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);


    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Unified shader has an error: Error: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

unsigned int loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Texture didn't load. File path: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}

unsigned loadTexture(const char* filePath, unsigned shader)
{
    unsigned texture = loadImageToTexture(filePath);
    glBindTexture(GL_TEXTURE_2D, texture); 
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(shader);
    unsigned uTexLoc = glGetUniformLocation(shader, "uTex");
    glUniform1i(uTexLoc, 0); 
    return texture;
}

void drawBackground(unsigned texture, unsigned shader, unsigned* VAO)
{
    glUseProgram(shader);
    glBindVertexArray(*VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

std::vector<float> generateCircleVertices(float centerX, float centerY, float radius, int numSegments)
{
    std::vector<float> vertices;

    vertices.push_back(centerX);
    vertices.push_back(centerY);

    for (int i = 0; i <= numSegments; ++i)
    {
        float angle = i * 2.0f * M_PI / numSegments;
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        vertices.push_back(x);
        vertices.push_back(y);
    }

    return vertices;
}

void drawSun(unsigned int shader, unsigned int *VAO, int size)
{
    glUseProgram(shader);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(*VAO);
    unsigned int timeLoc = glGetUniformLocation(shader, "time");
    glUniform1f(timeLoc, glfwGetTime());
    glDrawArrays(GL_TRIANGLE_FAN, 0, size);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
}

std::vector<float> generateHorizontalLines(int numLines) {
    std::vector<float> vertices;

    float spacing = 1.0f / numLines; 
    for (int i = 0; i < numLines; ++i) {
        float yPos = -1.0f + i * spacing;  
        vertices.push_back(-1.0f);
        vertices.push_back(yPos);  

        vertices.push_back(1.0f);   
        vertices.push_back(yPos);
    }

    return vertices;
}

std::vector<float> generateVerticalLines(int numLines) {
    std::vector<float> vertices;

    float spacing = 2.0f / numLines;
    for (int i = 0; i < numLines; ++i) {
        float xPos = -1.0f + i * spacing;
        float xPosStart = xPos + (numLines / -2.0f + i)*0.15;
        vertices.push_back(xPosStart);
        vertices.push_back(-1.0f);  
        vertices.push_back(xPos);
        vertices.push_back(0.0f);   
    }

    return vertices;
}

void drawGrid(unsigned int shader, unsigned int *horizontalVAO, unsigned int horizontalSize, unsigned int *verticalVAO, unsigned int verticalSize)
{
    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "isHorizontal"), 1);
    unsigned int timeLoc = glGetUniformLocation(shader, "time");
    glUniform1f(timeLoc, glfwGetTime());
    glBindVertexArray(*horizontalVAO);
    glDrawArrays(GL_LINES, 0, horizontalSize);


    glUniform1i(glGetUniformLocation(shader, "isHorizontal"), 0);
    timeLoc = glGetUniformLocation(shader, "time");
    glUniform1f(timeLoc, glfwGetTime());
    glBindVertexArray(*verticalVAO);
    glDrawArrays(GL_LINES, 0, verticalSize);
    glUseProgram(0);
}

float mapRange(float x, float sourceMin, float sourceMax, float targetMin, float targetMax) {
    return targetMin + ((x - sourceMin) / (sourceMax - sourceMin)) * (targetMax - targetMin);
}

void updateKnobPosition()
{
    knobPosition = mapRange(getCurrentSeek(), 0, 1, 0, 0.8);
}

void drawControls(unsigned shader, unsigned* VAO)
{
    glUseProgram(shader);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniform1i(glGetUniformLocation(shader, "isKnob"), 0);
    glBindVertexArray(*VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawArrays(GL_TRIANGLES, 4, 3);
    if (isMusicPlaying)
    {
        glDrawArrays(GL_TRIANGLE_STRIP, 7, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 11, 4);
    }else
    {
        glDrawArrays(GL_TRIANGLES, 15, 3);
    }
    glDrawArrays(GL_TRIANGLES, 18, 3);
    glDrawArrays(GL_TRIANGLE_STRIP, 21, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 25, 4);
    glUniform1i(glGetUniformLocation(shader, "isKnob"), 1);
    updateKnobPosition();
    glUniform1f(glGetUniformLocation(shader, "knobPosition"), knobPosition);
    glDrawArrays(GL_TRIANGLE_STRIP, 29, 4);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glUseProgram(0);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (isDragging) {
        float sliderX = 300.0f;
        float sliderWidth = 400.0f;
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        float seek = (xpos - sliderX) / sliderWidth;
        seek = std::clamp(seek, 0.0f, 1.0f);
        seekTo(seek);
        knobPosition = mapRange(xpos, sliderX, sliderX+ sliderWidth,0, 0.8);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        if(xpos >= 375 && xpos <= 425 && ypos >= 700 && ypos <= 750){
            playPreviousSong();
        }
        else if(xpos >= 480 && xpos <= 520 && ypos >= 700 && ypos <= 750) {
            togglePlayPause();
        }
        else if(xpos >= 575 && xpos <= 625 && ypos >= 700 && ypos <= 750) {
            playNextSong();
        }else if (xpos>=300 && xpos<=700)
        {
            isDragging = true;
        }

    }
	else{
        isDragging = false;
    }
    

}