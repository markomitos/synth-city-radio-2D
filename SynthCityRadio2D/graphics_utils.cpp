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
#include "glm/vec2.hpp"
#include <ft2build.h>
#include <glm/fwd.hpp>
#include <glm/matrix.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include FT_FREETYPE_H

#include <chrono>

float textOffset = 0.0f;
bool isDragging = false;
bool isDraggingVolume = false;
float knobPosition = 0.0f;
float volumeKnobPosition = 0.0f;

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

void init2cordRGBVAO(unsigned int* VAO, unsigned int* VBO, float vertices[], unsigned int size, unsigned int stride)
{
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);

    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void initTextVAO(unsigned int* VAO, unsigned int* VBO)
{
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
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


void updateVolumeKnobPosition()
{
    volumeKnobPosition = mapRange(getCurrentVolume(), 0, 100, 0, 0.5);
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

void drawVolumeControls(unsigned shader, unsigned* VAO)
{
    glUseProgram(shader);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniform1i(glGetUniformLocation(shader, "isKnob"), 0);
    glBindVertexArray(*VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glUniform1i(glGetUniformLocation(shader, "isKnob"), 1);
    updateVolumeKnobPosition();
    glUniform1f(glGetUniformLocation(shader, "knobPosition"), volumeKnobPosition);
    glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
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
    else if (isDraggingVolume)
    {
        float sliderX = 375.0f;
        float sliderWidth = 250.0f;
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        float seek = (xpos - sliderX) / sliderWidth;
        seek = std::clamp(seek, 0.0f, 1.0f);
        setCurrentVolume(seek*100);
        volumeKnobPosition = mapRange(xpos, sliderX, sliderX + sliderWidth, 0, 0.5);
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
        }
    	else if (xpos>=300 && xpos<=700 && ypos>= 790 && ypos<=820) {
            isDragging = true;
        }
        else if (xpos >= 375 && xpos <= 625 && ypos >= 840 && ypos <= 870) {
            isDraggingVolume = true;
        }

    }
	else{
        isDragging = false;
        isDraggingVolume = false;
    }
    

}

void RenderText(unsigned shader, std::string text, float x, float y, float scale, glm::vec3 color, unsigned* VAO, unsigned* VBO)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glm::mat4 projection = glm::ortho(0.0f, 1000.0f, 0.0f, 1000.0f, -1.0f, 1.0f);
    GLint projLoc = glGetUniformLocation(shader, "projection");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    // activate corresponding render state
    glUseProgram(shader);
    glUniform3f(glGetUniformLocation(shader, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(*VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, *VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderScrollingText(
    unsigned shader,
    const std::string& text,
    float x,
    float y,
    float scale,
    glm::vec3 color,
    unsigned* VAO,
    unsigned* VBO,
    float maxWidth,
    float scrollSpeed,
    float gapWidth)
{
    static float textOffset = 0.0f; 

    float textLength = 0.0f; 
    for (const char& c : text) {
        Character ch = Characters[c];
        textLength += (ch.Advance >> 6) * scale; 
    }

    if (textOffset > textLength + gapWidth) {
        textOffset = 0.0f;
    }

    textOffset += scrollSpeed;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glm::mat4 projection = glm::ortho(0.0f, 1000.0f, 0.0f, 1000.0f, -1.0f, 1.0f);
    GLint projLoc = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glUseProgram(shader);
    glUniform3f(glGetUniformLocation(shader, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(*VAO);

    float cursorX = -textOffset; 

    for (const char& c : text) {
        Character ch = Characters[c];

        float xpos = x + cursorX + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        if (cursorX > maxWidth) {
            break;
        }
        
        if (cursorX + w > 0.0f) {
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };


            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, *VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        cursorX += (ch.Advance >> 6) * scale;
    }


    cursorX = -textOffset + textLength + gapWidth; 
    for (const char& c : text) {
        Character ch = Characters[c];

        float xpos = x + cursorX + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        if (cursorX > maxWidth) {
            break;
        }

        if (cursorX + w > 0.0f) {
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, *VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        cursorX += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}




int initFreeType()
{
    FT_Library ft;
    FT_Face face;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }


    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void drawText(unsigned int shader, unsigned int* VAO, unsigned int* VBO)
{
    RenderScrollingText(
        shader,
        currentSong,
        230.0f,
        340.0f,
        1.0f, 
        glm::vec3(1, 1, 1),
        VAO,
        VBO,
        540.0f, 
        0.5f,
        100.0f
    );
	RenderText(shader, currentDuration, 230.0f, 190.0f, 0.5f, glm::vec3(1, 1, 1), VAO, VBO);
    RenderText(shader, totalDuration, 710.0f, 190.0f, 0.5f, glm::vec3(1, 1, 1), VAO, VBO);

}