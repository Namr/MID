#ifndef TIFFGL_H
#define TIFFGL_H

#include <GL/glew.h>

#include <string>
#include <iostream>
#include <tiffio.h>

class TiffGL
{
  public:
    int depth = 0;
    int width = 0;
    int height = 0;
    uint16_t *data;
    GLuint texture;
    TiffGL(std::string filepath);
};
#endif