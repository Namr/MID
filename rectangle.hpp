#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <cstdlib>
#include <GL/glew.h>
#include <iostream>

class Vertex
{
  public:
    float x, y;
    Vertex(float xCoord, float yCoord);
    Vertex();
};

class Rectangle
{
  unsigned int VAO, VBO, EBO;
  GLuint triangles[6];
  float vertices[8];
  GLuint shaderProgram;
  GLuint loadShader(const char *filepath, GLenum type);
  public:
    Vertex tl, tr, bl, br;
    Rectangle(Vertex topLeft, Vertex bottomRight);
    Rectangle();
    int intersect(Rectangle rect);
    void graphicsInit();
    void resize();
    void render();
};

#endif