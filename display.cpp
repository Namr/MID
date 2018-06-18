#include "display.hpp"

//ARGUEMENTS: The cordinates of the top left vertex of the dispaly
Display::Display(float tlX, float tlY)
{
    //generate vertex position based on the top left cordinates
    vertices[0] = tlX;
    vertices[1] = tlY;

    vertices[4] = tlX + 1.0f;
    vertices[5] = tlY;

    vertices[8] = tlX + 1.0f;
    vertices[9] = tlY - 1.0f;

    vertices[12] = tlX;
    vertices[13] = tlY - 1.0f;

    //set texture coordinates
    vertices[2] = 0.0f;
    vertices[3] = 0.0f;

    vertices[6] = 1.0f;
    vertices[7] = 0.0f;

    vertices[10] = 1.0f;
    vertices[11] = 1.0f;

    vertices[14] = 0.0f;
    vertices[15] = 1.0f;

    //generate triangle data for a square
    triangles[0] = 0;
    triangles[1] = 1;
    triangles[2] = 2;
    triangles[3] = 2;
    triangles[4] = 3;
    triangles[5] = 0;

    //generate and bind the buffers assosiated with this display in order to assign vertices and triangles
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //load shaders from their files
    GLuint vertexShader = loadShader("shaders/vertex.glsl", GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShader("shaders/fragment.glsl", GL_FRAGMENT_SHADER);

    // compile the GPU programs
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    // catch any errors from compilation
    GLint success;
    GLchar infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR VERTEX SHADER COMPILE FAILED\n"
                  << infoLog << std::endl;
    };

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR FRAGMENT SHADER COMPILE FAILED\n"
                  << infoLog << std::endl;
    };

    // create a program from the shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    //bind output vars, link and use the program
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //send the verticies to the GPU through the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

    // pass and bind triangle data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles), &triangles[0], GL_STATIC_DRAW);

    //send the formatting of the vertex data to the GPU
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texCoords");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shaderProgram, "textureArray"), 0);

    glUniform1f(glGetUniformLocation(shaderProgram, "zoom"), 1.5);
    glUniform1f(glGetUniformLocation(shaderProgram, "xOffset"), 0.5f);
    glUniform1f(glGetUniformLocation(shaderProgram, "yOffset"), 0.1f);
}

void Display::useNIFTI(NIFTI file)
{
}

void Display::update()
{
    glActiveTexture(GL_TEXTURE0);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glUniform1i(glGetUniformLocation(shaderProgram, "layer"), layer);

    glDrawElements(GL_TRIANGLES, sizeof(triangles) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
}

GLuint Display::loadShader(const char *filepath, GLenum type)
{
    FILE *file = fopen(filepath, "rb");
    if (!file)
    {
        return 0;
    }

    long len;
    if (fseek(file, 0, SEEK_END) != 0 || (len = ftell(file)) == -1L)
    {
        fclose(file);
        return 0;
    }
    rewind(file);

    char *buffer = (char *)malloc(len);
    if (fread(buffer, 1, len, file) != len)
    {
        fclose(file);
        free(buffer);
        return 0;
    }
    fclose(file);

    GLuint shader = glCreateShader(type);
    if (shader == 0)
    {
        free(buffer);
        return 0;
    }

    glShaderSource(shader, 1, (const char *const *)&buffer, (GLint *)&len);
    free(buffer);
    return shader;
}