#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>


class Display
{
    unsigned int VAO, VBO, EBO;
    GLuint loadShader(const char *filepath, GLenum type);
    GLuint shaderProgram;
    float vertices[8];
    GLuint triangles[6];
public:
    Display(float tlX, float tlY);
    void update();
};