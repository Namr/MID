#include "parsers/tiffgl.hpp"

TiffGL::TiffGL(std::string filepath)
{
    //find the number of layers in the tiff file
    TIFF *temp = TIFFOpen(filepath.c_str(), "r");
    if (temp)
    {
        do
        {
            depth++;
        } while (TIFFReadDirectory(temp));
        TIFFClose(temp);
    }

    TIFF *image = TIFFOpen(filepath.c_str(), "r");
    if (image)
    {
        TIFFGetField(image, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(image, TIFFTAG_IMAGELENGTH, &height);
        uint16_t *buf = (uint16_t *)_TIFFmalloc(width * sizeof(uint16_t));
        data = new uint16_t[height * width * depth];

        for (int layer = 0; layer < depth; layer++)
        {
            for (int row = 0; row < height; row++)
            {
                TIFFReadScanline(image, (tdata_t)buf, row);
                for (int col = 0; col < width; col++)
                    data[(layer * width * height) + (row * width) + col] = buf[col];
            }
            TIFFReadDirectory(image);
        }

        _TIFFfree((tdata_t)buf);
        TIFFClose(image);

        //generate the buffers for the texture data which will be supplied by another function
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

        //allocate memory for the incoming data
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_R16, width, height, depth);

        //upload data, the 0s are specifiying to start the upload from the very start of the data
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, depth, GL_RED, GL_UNSIGNED_SHORT, data);

        //Set Texture Parameters
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else
        std::cout << "ERROR: IMAGE UNABLE TO LOAD" << std::endl;
}