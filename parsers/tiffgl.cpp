#include "parsers/tiffgl.hpp"

TiffGL::TiffGL(std::string filepath)
{

    //find and store all tiff file paths in selected directory
    for (auto &p : std::filesystem::directory_iterator("D:/171002_R_0-148NA_zoom0-8X_whole_15-40-40"))
        imageList.push_back(p);

    //get the original image specs
    TIFF *origin_image = TIFFOpen(imageList[0].string().c_str(), "r");
    if (origin_image)
    {
        TIFFGetField(origin_image, TIFFTAG_IMAGEWIDTH, &origin_width);
        TIFFGetField(origin_image, TIFFTAG_IMAGELENGTH, &origin_height);
        origin_depth = imageList.size();
        TIFFClose(origin_image);
    }

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
        glBindTexture(GL_TEXTURE_3D, texture);

        //allocate memory for the incoming data
        glTexStorage3D(GL_TEXTURE_3D, 1, GL_R16, width, height, depth);

        //upload data, the 0s are specifiying to start the upload from the very start of the data
        glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, width, height, depth, GL_RED, GL_UNSIGNED_SHORT, data);

        free(data);

        //Set Texture Parameters
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else
        std::cout << "ERROR: IMAGE UNABLE TO LOAD" << std::endl;
}

void TiffGL::getRegion(Rectangle pixelCoords, int layer, int view)
{
    if (view == 0)
    {
        TIFF *image = TIFFOpen(imageList[layer].string().c_str(), "r");
        if (image)
        {
            //height and width of the selected area IN THE ORIGINAL IMAGES PIXEL COORDINATES
            int height = (int)abs((int)pixelCoords.tl.y - (int)pixelCoords.bl.y);
            int width = (int)abs((int)pixelCoords.tl.x - (int)pixelCoords.tr.x);

            //original length of the image
            int length = 0;
            TIFFGetField(image, TIFFTAG_IMAGELENGTH, &length);

            //the buffer is the size of a row in the original image, however the raster is the size of the selected area (its the image to be displayed)
            uint16_t *buf = (uint16_t *)_TIFFmalloc(length * sizeof(uint16_t));
            uint16_t *raster = new uint16_t[height * width];

            //iterate over every pixel in the original image that corresponds to the selected area and put it in the raster
            int y = 0;
            for (int row = (int)pixelCoords.tl.y; row < (int)pixelCoords.bl.y; row++)
            {
                TIFFReadScanline(image, (tdata_t)buf, row);
                int x = 0;
                for (int col = (int)pixelCoords.tl.x; col < (int)pixelCoords.tr.x; col++)
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
            glDeleteTextures(1, &subTexture);
            glGenTextures(1, &subTexture);
            glBindTexture(GL_TEXTURE_2D, subTexture);

            //allocate memory for the incoming data
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_R16, width, height);

            //upload data, the 0s are specifiying to start the upload from the very start of the data
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_SHORT, raster);

            free(raster);

            //Set Texture Parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        else
            std::cout << "ERROR: IMAGE UNABLE TO LOAD" << std::endl;
    }
    else if (view == 1)
    {

        //height and width of the selected area IN THE ORIGINAL IMAGES PIXEL COORDINATES
        int height = (int)abs((int)pixelCoords.tl.y - (int)pixelCoords.bl.y);
        int width = (int)abs((int)pixelCoords.tl.x - (int)pixelCoords.tr.x); 

        //original length of the image
        int length = 0;

        uint16_t *raster = new uint16_t[height * width];

        //store all the slices that correspond to the selected area
        std::vector<TIFF *> sliceSet;
        for (int i = (int)pixelCoords.tl.y; i < (int)pixelCoords.bl.y; i++)
        {
            TIFF *slice = TIFFOpen(imageList[i].string().c_str(), "r");
            if (slice)
            {
                TIFFGetField(slice, TIFFTAG_IMAGELENGTH, &length);
                sliceSet.push_back(slice);
            }
        }

        uint16_t *buf = (uint16_t *)_TIFFmalloc(length * sizeof(uint16_t));

        for (unsigned int row = 0; row < sliceSet.size(); row++)
        {
            int x = 0;
            for (int col = (int)pixelCoords.tl.x; col < (int)pixelCoords.tr.x; col++)
            {
                TIFFReadScanline(sliceSet[row], (tdata_t)buf, col);
                raster[(row * width) + x] = buf[layer];
                x++;
            }
            TIFFClose(sliceSet[row]);
        }

        _TIFFfree((tdata_t)buf);

        //generate the buffers for the texture data which will be supplied by another function
        glActiveTexture(GL_TEXTURE2);
        glDeleteTextures(1, &subTexture1);
        glGenTextures(1, &subTexture1);
        glBindTexture(GL_TEXTURE_2D, subTexture1);

        //allocate memory for the incoming data
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_R16, width, height);

        //upload data, the 0s are specifiying to start the upload from the very start of the data
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_SHORT, raster);

        free(raster);

        //Set Texture Parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else if (view == 2)
    {
        //height and width of the selected area IN THE ORIGINAL IMAGES PIXEL COORDINATES
        int height = (int)abs((int)pixelCoords.tl.y - (int)pixelCoords.bl.y);
        int width = (int)abs((int)pixelCoords.tl.x - (int)pixelCoords.tr.x); 

        //original length of the image
        int length = 0;

        uint16_t *raster = new uint16_t[height * width];

        //store all the slices that correspond to the selected area
        std::vector<TIFF *> sliceSet;
        for (int i = (int)pixelCoords.tl.y; i < (int)pixelCoords.bl.y; i++)
        {
            TIFF *slice = TIFFOpen(imageList[i].string().c_str(), "r");
            if (slice)
            {
                TIFFGetField(slice, TIFFTAG_IMAGELENGTH, &length);
                sliceSet.push_back(slice);
            }
        }

        uint16_t *buf = (uint16_t *)_TIFFmalloc(length * sizeof(uint16_t));

        for (unsigned int row = 0; row < sliceSet.size(); row++)
        {
            int x = 0;
            TIFFReadScanline(sliceSet[row], (tdata_t)buf, layer);
            for (int col = (int)pixelCoords.tl.x; col < (int)pixelCoords.tr.x; col++)
            {
                raster[(row * width) + x] = buf[col];
                x++;
            }
            TIFFClose(sliceSet[row]);
        }

        _TIFFfree((tdata_t)buf);

        //generate the buffers for the texture data which will be supplied by another function
        glActiveTexture(GL_TEXTURE3);
        glDeleteTextures(1, &subTexture2);
        glGenTextures(1, &subTexture2);
        glBindTexture(GL_TEXTURE_2D, subTexture2);

        //allocate memory for the incoming data
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_R16, width, height);

        //upload data, the 0s are specifiying to start the upload from the very start of the data
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_SHORT, raster);

        free(raster);

        //Set Texture Parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}