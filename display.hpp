#ifndef DISPLAY_H
#define DISPLAY_H

#include <GL/glew.h>

#include <iostream>
#include "parsers/nifti.hpp"
#include "rectangle.h"

class Display
{
    unsigned int VAO, VBO, EBO;
    GLuint loadShader(const char *filepath, GLenum type);
    GLuint shaderProgram, texture;
    GLuint triangles[6];
    float vertices[16];
  public:
    Display(float tlX, float tlY);
    void update();
    void resize();
    Rectangle position;
    Rectangle textureCoords;
    int layer = 2;
    float zoom = 1.0f;
    float xOffset = 0.0f;
    float yOffset = 0.0f;
};

#endif