#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <tiffio.h>
#include <iostream>
#include <vector>
#include <filesystem>

#include "MID.h"
#include "display.hpp"
#include "parsers/nifti.hpp"
#include "parsers/tiffgl.hpp"
// Function prototypes
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int curlayer = 0;
int upperLayer = 0;

void resizeTIFF(std::string path)
{
    //find and store all tiff file paths in selected directory
    std::vector<std::filesystem::path> imageList;
    for (auto &p : std::filesystem::directory_iterator(path))
        imageList.push_back(p);

    //find microscope image details (width, height, depth, and colorspace)
    int height = 0;
    int width = 0;
    int depth = imageList.size();
    int colorspace = 0;
    TIFF *temp = TIFFOpen(imageList[0].string().c_str(), "r");
    if (temp)
    {
        TIFFGetField(temp, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(temp, TIFFTAG_IMAGELENGTH, &height);
        TIFFGetField(temp, TIFFTAG_PHOTOMETRIC, &colorspace);

        TIFFClose(temp);
    }

    std::cout << "Height: " << height << " Width: " << width << " Depth: " << depth << " ColorSpace: " << colorspace << std::endl;

    //create a compressed version of the entire data to provide an overview image
    int ratio = 5;
    TIFF *overviewImage = TIFFOpen("overview.tif", "w");

    if (overviewImage)
    {
        for (int i = 0; i < (depth / ratio); i++)
        {
            //read 5(or whatever the ratio is) images fully into memory to compress them into a single image
            std::vector<uint16_t *> cachedImages;

            for (int w = 0; w < ratio; w++)
            {
                TIFF *image = TIFFOpen(imageList[(i * ratio) + w].string().c_str(), "r");
                if (image)
                {
                    uint16_t *buf = (uint16_t *)_TIFFmalloc(width * sizeof(uint16_t));
                    uint16_t *raster = new uint16_t[height * width];

                    for (int row = 0; row < height; row++)
                    {
                        TIFFReadScanline(image, (tdata_t)buf, row);
                        for (int col = 0; col < width; col++)
                            raster[(row * width) + col] = buf[col];
                    }

                    cachedImages.push_back(raster);
                    _TIFFfree((tdata_t)buf);
                    TIFFClose(image);
                }
                else
                    std::cout << "ERROR: IMAGE UNABLE TO LOAD" << std::endl;
            }

            //after the images are cached make a new page in the new TIFF and fill it with the averaged information
            TIFFSetField(overviewImage, TIFFTAG_IMAGEWIDTH, (width / ratio));
            TIFFSetField(overviewImage, TIFFTAG_IMAGELENGTH, (height / ratio));
            TIFFSetField(overviewImage, TIFFTAG_SAMPLESPERPIXEL, 1);
            TIFFSetField(overviewImage, TIFFTAG_BITSPERSAMPLE, 16);
            TIFFSetField(overviewImage, TIFFTAG_ROWSPERSTRIP, height);
            TIFFSetField(overviewImage, TIFFTAG_ORIENTATION, (int)ORIENTATION_TOPLEFT);
            TIFFSetField(overviewImage, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
            TIFFSetField(overviewImage, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
            TIFFSetField(overviewImage, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

            TIFFSetField(overviewImage, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
            TIFFSetField(overviewImage, TIFFTAG_PAGENUMBER, i, (depth / ratio));

            //iterate over this pages pixels in the new images in order to calculate them
            for (int y = 0; y < (height / ratio); y++)
            {
                uint16_t *buf = new uint16_t[(width / ratio)];

                for (int x = 0; x < (width / ratio); x++)
                {
                    int averaged_value = 0;
                    //iterate over the 5x5x5 area in the original image that corresponds to the pixel in the compressed version and average all of those pixels
                    for (int layer = 0; layer < ratio; layer++)
                    {
                        for (int w = 0; w < ratio; w++)
                        {
                            for (int v = 0; v < ratio; v++)
                            {
                                averaged_value += cachedImages.at(layer)[(y * width * ratio) + w + (x * ratio + v)];
                            }
                        }
                    }
                    averaged_value /= (int)pow(ratio, 3);
                    buf[x] = averaged_value;
                }
                TIFFWriteScanline(overviewImage, buf, y);
            }
            TIFFWriteDirectory(overviewImage);

            //free the data that was in the cache
            for (int layer = 0; layer < ratio; layer++)
            {
                free(cachedImages.at(layer));
            }
            std::cout << (i / (float)(depth / ratio)) * 100.0f << "%" << std::endl;
        }
    }
    TIFFClose(overviewImage);
}

int main(int argc, char **argv)
{

    //compression mode
    if (argc == 2 && std::string(argv[1]) == "-c")
    {
        resizeTIFF("D:/171002_R_0-148NA_zoom0-8X_whole_15-40-40");
        std::cout << "Compression Complete" << std::endl;
        return 0;
    }

    // Init GLFW and Set all the required options
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Create a GLFW window and GL context
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "MID", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Initialize GLEW and set it to use modern OpenGL
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    //image view initilization
    //NIFTI file("C:/Users/vrdem/Documents/Ellie_Sarah_Top.nii");
    TiffGL file("C:/Users/vrdem/Documents/GitHub/MID/build/Debug/overview.tif");
    Display display(-1.0f, 1.0f);

    display.image = &file;

    upperLayer = file.depth;

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Define the viewport dimensions in case the window size was changed by the user
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        //Clear the screen with a background color
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        display.layer = curlayer;
        //render objects
        display.update(window);

        //catch errors
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
            std::cout << "OPENGL ERROR: " << err << std::endl;

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    std::cout << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(curlayer + yoffset >= 0 || curlayer + yoffset <= upperLayer)
        curlayer += (int) yoffset;
}