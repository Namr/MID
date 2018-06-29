#ifndef DISPLAY_H
#define DISPLAY_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "MID.h"
#include "parsers/nifti.hpp"
#include "rectangle.h"

class Display
{
    unsigned int VAO, VBO, EBO;
    GLuint loadShader(const char *filepath, GLenum type);
    GLuint shaderProgram, texture;
    GLuint triangles[6];
    float vertices[16];
    int highlighting = 0;
    Rectangle highlighter;
  public:
    Display(float tlX, float tlY);
    void update(GLFWwindow* window);
    void resize();
    Rectangle position;
    Rectangle textureCoords;
    int layer = 100;
    float zoom = 1.0f;
    float xOffset = 0.0f;
    float yOffset = 0.0f;
};

#endif