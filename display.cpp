#include "display.hpp"

void display::init()
{
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
    std::cout << "ERROR VERTEX SHADER COMPILE FAILED\n" << infoLog << std::endl;
  };

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR FRAGMENT SHADER COMPILE FAILED\n" << infoLog << std::endl;
  };

  // create a program from the shaders
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  //bind output vars, link and use the program
  glBindFragDataLocation(shaderProgram, 0, "outColor");
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);


}

void display::update()
{
}

GLuint loadShader(const char *filepath, GLenum type)
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