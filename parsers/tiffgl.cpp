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
        glGenTextures(1, &subTexture);
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

void TiffGL::getRegion(Rectangle pixelCoords, int layer)
{
    //find and store all tiff file paths in selected directory
    std::vector<std::filesystem::path> imageList;
    for (auto &p : std::filesystem::directory_iterator("D:/171002_R_0-148NA_zoom0-8X_whole_15-40-40"))
        imageList.push_back(p);

    TIFF *image = TIFFOpen(imageList[layer].string().c_str(), "r");
    if (image)
    {
        int height = abs(pixelCoords.tl.y - pixelCoords.bl.y);
        int width = abs(pixelCoords.tl.x - pixelCoords.tr.x);

        int length = 0;
        TIFFGetField(image, TIFFTAG_IMAGELENGTH, &length);

        uint16_t *buf = (uint16_t *)_TIFFmalloc(length * sizeof(uint16_t));
        uint16_t *raster = new uint16_t[height * width];

        int y = 0;
        for (int row = pixelCoords.tl.y; row < pixelCoords.bl.y; row++)
        {
            TIFFReadScanline(image, (tdata_t)buf, row);
            int x = 0;
            for (int col = pixelCoords.tl.x; col < pixelCoords.tr.x; col++)
            {
                raster[(y * width) + x] = buf[col];
                x++;
            }
            y++;
        }

        _TIFFfree((tdata_t)buf);
        TIFFClose(image);

        //generate the buffers for the texture data which will be supplied by another function
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, subTexture);

        //allocate memory for the incoming data
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_R16, width, height);

        //upload data, the 0s are specifiying to start the upload from the very start of the data
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_SHORT, raster);

        //Set Texture Parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else
        std::cout << "ERROR: IMAGE UNABLE TO LOAD" << std::endl;
}