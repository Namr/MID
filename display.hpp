#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

class display
{
    unsigned int VAO, VBO, EBO;
    GLuint loadShader(const char *filepath, GLenum type);
    GLuint shaderProgram;
    
public:
    void init();
    void update();
};