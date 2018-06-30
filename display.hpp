#ifndef DISPLAY_H
#define DISPLAY_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "MID.h"
#include "parsers/nifti.hpp"
#include "rectangle.h"
#include "parsers/tiffgl.hpp"

class Display
{
    unsigned int VAO, VBO, EBO;
    GLuint loadShader(const char *filepath, GLenum type);
    GLuint shaderProgram, texture;
    GLuint triangles[6];
    float vertices[24];
    int highlighting = 0;
    float detailMix = 0.0f;
    Rectangle highlighter;
  public:
    Display(float tlX, float tlY);
    void update(GLFWwindow* window);
    void resize();
    Rectangle position;
    Rectangle textureCoords;
    Rectangle pixelCoords;
    Rectangle standardTextureCoords;
    TiffGL *image;
    int layer = 0;
};

#endif