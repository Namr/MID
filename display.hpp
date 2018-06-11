#ifndef DISPLAY_H
#define DISPLAY_H

#include <GL/glew.h>

#include <iostream>
#include "parsers/nifti.hpp"

class Display
{
    unsigned int VAO, VBO, EBO;
    GLuint loadShader(const char *filepath, GLenum type);
    GLuint shaderProgram, texture;
    float vertices[16];
    GLuint triangles[6];
  public:
    Display(float tlX, float tlY);
    void useNIFTI(NIFTI file);
    void update();
    int layer = 2;
};

#endif