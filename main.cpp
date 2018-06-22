#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <tiffio.h>
#include <iostream>
#include <vector>
#include <filesystem>

#include "MID.h"
#include "display.hpp"
#include "parsers/nifti.hpp"

// Function prototypes
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

int main(int argc, char **argv)
{

    //compression mode
    if (argc == 2 && std::string(argv[1]) == "-c")
    {
        //find and store all tiff file paths in selected directory
        std::vector<std::filesystem::path> imageList;
        std::string path = "D:/171002_R_0-148NA_zoom0-8X_whole_15-40-40";
        for (auto &p : std::filesystem::directory_iterator(path))
            imageList.push_back(p);

        //find microscope image details
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
            for (int i = 0; i < depth / ratio; i++)
            {
                std::vector<TIFF *> cachedImages;
                for (int w = 0; w < cachedImages.size(); w++)
                {
                    TIFF *image = TIFFOpen(imageList[(i * 5) + w].string().c_str(), "r");
                    cachedImages.push_back(image);
                }
                
            }
        }
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

    // Initialize GLEW and set it to use modern OpenGL
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    //image view initilization
    NIFTI file("C:/Users/vrdem/Documents/Ellie_Sarah_Top.nii");
    Display display(-1.0f, 1.0f);

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