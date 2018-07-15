#ifndef DISPLAY_H
#define DISPLAY_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "MID.h"
#include "parsers/nifti.hpp"
#include "rectangle.hpp"
#include "parsers/tiffgl.hpp"

class ScreenObject
{
public:
  Rectangle pixelSpace;
  Rectangle screenSpace;
  Rectangle textureSpace;
  Rectangle relativeSpace;

  ScreenObject(Rectangle pixel);
  ScreenObject();
  void setScreenSpace(Rectangle pixelReference);
  void setTextureSpace(Rectangle reference);
  void setRelativeSpace(Rectangle reference);
};

class Display
{
  unsigned int VAO, VBO, EBO;
  GLuint loadShader(const char *filepath, GLenum type);
  GLuint shaderProgram;
  GLuint triangles[6];
  float vertices[24];
  int highlighting = 0;
  float detailMix = 0.0f;
  int view;
  ScreenObject highlighter;

public:
  Display(float tlX, float tlY, int orientation);
  void update(GLFWwindow *window);
  void resize();
  Rectangle position;
  Rectangle pixelCoords, textureCoords, standardTextureCoords;
  Vertex selectedPosition;
  TiffGL *image;
  int layer = 0;
};

#endif