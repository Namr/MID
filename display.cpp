#include "display.hpp"

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//ARGUEMENTS: The cordinates of the top left vertex of the dispaly
Display::Display(float tlX, float tlY)
{
    //generate vertex position, and texture coords based on the top left cordinates
    position = Rectangle(Vertex(tlX, tlY), Vertex(tlX + 1.0f, tlY - 1.0f));
    textureCoords = Rectangle(Vertex(0.0, 0.0), Vertex(1.0, 1.0));

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
    resize();

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
}

void Display::resize()
{
    //set vertex points based on Rectangle object
    vertices[0] = position.tl.x;
    vertices[1] = position.tl.y;
    vertices[4] = position.tr.x;
    vertices[5] = position.tr.y;
    vertices[8] = position.br.x;
    vertices[9] = position.br.y;
    vertices[12] = position.bl.x;
    vertices[13] = position.bl.y;

    //set texture coordinates based on Rectanlge object
    vertices[2] = textureCoords.tl.x;
    vertices[3] = textureCoords.tl.y;

    vertices[6] = textureCoords.tr.x;
    vertices[7] = textureCoords.tr.y;

    vertices[10] = textureCoords.br.x;
    vertices[11] = textureCoords.br.y;

    vertices[14] = textureCoords.bl.x;
    vertices[15] = textureCoords.bl.y;

    //send data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_DYNAMIC_DRAW);
}

void Display::update(GLFWwindow *window)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    //covert cursor position to screen space (normalized coords between -1 and 1)
    xpos = map((float)xpos, 0.0f, WIDTH, -1.0f, 1.0f);
    ypos = map((float)ypos, 0.0f, HEIGHT, 1.0f, -1.0f);

    /*
    find the vertical and horizontal distances between the top left corner of the display
    and the cursor, this distances is the same as the cursors local position on the display,
    however since the display is smaller than the screen, the bounds are position indifferent
    but not scale indifferent so they must be multiplied up to become normalized coordinates
    */
    xpos = std::abs(xpos - position.tl.x);
    ypos = std::abs(ypos - position.tl.y);

    int mouse0 = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (mouse0 == GLFW_PRESS && highlighting == 0)
    {
        highlighter = Rectangle();
        highlighter.tl.x = map((float)xpos, 0.0f, 1.0f, textureCoords.bl.x, textureCoords.br.x);
        highlighter.tl.y = map((float)ypos, 0.0f, 1.0f, textureCoords.tr.y, textureCoords.br.y);
        highlighting = 1;
    }
    if (mouse0 == GLFW_RELEASE && highlighting == 1)
    {
        highlighter.br.x = map((float)xpos, 0.0f, 1.0f, textureCoords.bl.x, textureCoords.br.x);
        highlighter.br.y = map((float)ypos, 0.0f, 1.0f, textureCoords.tr.y, textureCoords.br.y);

        highlighter = Rectangle(highlighter.tl, highlighter.br);
        textureCoords = highlighter;
        resize();
        highlighting = 0;
    }

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