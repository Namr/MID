#ifndef NIFTI_H
#define NIFTI_H

#include <GL/glew.h>

#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>

class NIFTI
{
  public:
    int16_t dimNum, width, height, depth, time, bitsPerPixel, bytesPerPixel;
    float voxelWidth, voxelHeight, voxelDepth, vox_offset;
    GLubyte *data;
    GLuint texture;
    NIFTI(std::string filepath);
};

template <class T>
void endswap(T *objp)
{
    unsigned char *memp = reinterpret_cast<unsigned char *>(objp);
    std::reverse(memp, memp + sizeof(T));
}

#endif