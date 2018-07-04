#ifndef TIFFGL_H
#define TIFFGL_H

#include <GL/glew.h>

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <tiffio.h>

#include "rectangle.h"

class TiffGL
{
  public:
    int depth = 0;
    int width = 0;
    int height = 0;
    uint16_t *data;
    GLuint texture;
    GLuint subTexture;
    TiffGL(std::string filepath);
    void getRegion(Rectangle pixelCoords, int layer);
};
#endif