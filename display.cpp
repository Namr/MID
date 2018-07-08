#include "display.hpp"

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//ARGUEMENTS: The cordinates of the top left vertex of the dispaly
Display::Display(float tlX, float tlY, int orientation)
{
    view = orientation;

    //generate vertex position, and texture coords based on the top left cordinates
    position = Rectangle(Vertex(tlX, tlY), Vertex(tlX + 1.0f, tlY - 1.0f));
    textureCoords = Rectangle(Vertex(0.0, 0.0), Vertex(1.0, 1.0));

    //these texture coords never change so the detailed image can be rendered onto them
    standardTextureCoords = Rectangle(Vertex(0.0, 0.0), Vertex(1.0, 1.0));

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
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texCoords");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(2 * sizeof(float)));

    GLint standardTexAttrib = glGetAttribLocation(shaderProgram, "standardTexCoords");
    glEnableVertexAttribArray(standardTexAttrib);
    glVertexAttribPointer(standardTexAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(4 * sizeof(float)));

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shaderProgram, "textureArray"), 0);
}

void Display::resize()
{
    //set vertex points based on Rectangle object
    vertices[0] = position.tl.x;
    vertices[1] = position.tl.y;

    vertices[6] = position.tr.x;
    vertices[7] = position.tr.y;

    vertices[12] = position.br.x;
    vertices[13] = position.br.y;

    vertices[18] = position.bl.x;
    vertices[19] = position.bl.y;

    //set texture coordinates based on Rectangle object
    vertices[2] = textureCoords.tl.x;
    vertices[3] = textureCoords.tl.y;

    vertices[8] = textureCoords.tr.x;
    vertices[9] = textureCoords.tr.y;

    vertices[14] = textureCoords.br.x;
    vertices[15] = textureCoords.br.y;

    vertices[20] = textureCoords.bl.x;
    vertices[21] = textureCoords.bl.y;

    //set unchanging texture coordinates based on Rectangle object
    vertices[4] = standardTextureCoords.tl.x;
    vertices[5] = standardTextureCoords.tl.y;

    vertices[10] = standardTextureCoords.tr.x;
    vertices[11] = standardTextureCoords.tr.y;

    vertices[16] = standardTextureCoords.br.x;
    vertices[17] = standardTextureCoords.br.y;

    vertices[22] = standardTextureCoords.bl.x;
    vertices[23] = standardTextureCoords.bl.y;

    //send data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_DYNAMIC_DRAW);
}

void Display::update(GLFWwindow *window)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    int mouse0 = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (mouse0 == GLFW_PRESS && highlighting == 0)
    {
        highlighter.pixelSpace.tl.x = (float)xpos;
        highlighter.pixelSpace.tl.y = (float)ypos;

        highlighting = 1;
    }
    if (mouse0 == GLFW_RELEASE && highlighting == 1)
    {
        highlighter.pixelSpace.br.x = (float)xpos;
        highlighter.pixelSpace.br.y = (float)ypos;

        highlighter.pixelSpace = Rectangle(highlighter.pixelSpace.tl, highlighter.pixelSpace.br);

        highlighter = ScreenObject(highlighter.pixelSpace);
        highlighter.setRelativeSpace(position);
        highlighter.setTextureSpace(textureCoords);

        if (highlighter.screenSpace.intersect(position) == 1)
        {
            textureCoords = highlighter.textureSpace;

            //find the pixel space coordiantes for the current zoom box
            pixelCoords.tl.x = map(highlighter.relativeSpace.tl.x, 0.0f, 1.0f, 0.0f, (float)image->origin_width);
            pixelCoords.tl.y = map(highlighter.relativeSpace.tl.y, 0.0f, 1.0f, 0.0f, (float)image->origin_height);
            pixelCoords.br.x = map(highlighter.relativeSpace.br.x, 0.0f, 1.0f, 0.0f, (float)image->origin_width);
            pixelCoords.br.y = map(highlighter.relativeSpace.br.y, 0.0f, 1.0f, 0.0f, (float)image->origin_height);

            pixelCoords = Rectangle(pixelCoords.tl, pixelCoords.br);

            image->getRegion(pixelCoords, layer * 5);
            detailMix = 1.0f;

            glActiveTexture(GL_TEXTURE1);
            glUniform1i(glGetUniformLocation(shaderProgram, "detail"), 1);

            resize();
        }

        highlighting = 0;
    }

    //reset zoom and pan of the image to go back to top level view
    int reset = glfwGetKey(window, GLFW_KEY_R);
    if (reset == GLFW_PRESS)
    {
        detailMix = 0.0f;
        textureCoords = Rectangle(Vertex(0.0, 0.0), Vertex(1.0, 1.0));
        resize();
    }

    glActiveTexture(GL_TEXTURE0);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glUniform1f(glGetUniformLocation(shaderProgram, "layer"), (float)layer / image->depth);
    glUniform1f(glGetUniformLocation(shaderProgram, "detailMix"), detailMix);
    glUniform1i(glGetUniformLocation(shaderProgram, "view"), view);

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

ScreenObject::ScreenObject() {}

ScreenObject::ScreenObject(Rectangle pixel)
{
    pixelSpace = pixel;

    screenSpace.tl.x = map(pixelSpace.tl.x, 0.0f, WIDTH, -1.0f, 1.0f);
    screenSpace.tl.y = map(pixelSpace.tl.y, 0.0f, HEIGHT, 1.0f, -1.0f);
    screenSpace.br.x = map(pixelSpace.br.x, 0.0f, WIDTH, -1.0f, 1.0f);
    screenSpace.br.y = map(pixelSpace.br.y, 0.0f, HEIGHT, 1.0f, -1.0f);

    screenSpace = Rectangle(screenSpace.tl, screenSpace.br);
}

void ScreenObject::setRelativeSpace(Rectangle reference)
{
    relativeSpace.tl.x = std::abs(screenSpace.tl.x - reference.tl.x);
    relativeSpace.tl.y = std::abs(screenSpace.tl.y - reference.tl.y);

    relativeSpace.br.x = std::abs(screenSpace.br.x - reference.tl.x);
    relativeSpace.br.y = std::abs(screenSpace.br.y - reference.tl.y);

    relativeSpace = Rectangle(relativeSpace.tl, relativeSpace.br);
}

void ScreenObject::setTextureSpace(Rectangle reference)
{

    textureSpace.tl.x = map(relativeSpace.tl.x, 0.0f, 1.0f, reference.bl.x, reference.br.x);
    textureSpace.tl.y = map(relativeSpace.tl.y, 0.0f, 1.0f, reference.tr.y, reference.br.y);
    textureSpace.br.x = map(relativeSpace.br.x, 0.0f, 1.0f, reference.bl.x, reference.br.x);
    textureSpace.br.y = map(relativeSpace.br.y, 0.0f, 1.0f, reference.tr.y, reference.br.y);

    textureSpace = Rectangle(textureSpace.tl, textureSpace.br);
}