cmake_minimum_required(VERSION 3.5)

project(MID)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

IF (WIN32)
  list(APPEND CMAKE_PREFIX_PATH "C:/Program Files (x86)/glew")
  list(APPEND CMAKE_PREFIX_PATH "C:/Program Files (x86)/tiff")
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(TIFF REQUIRED)

include_directories(${CMAKE_SOURCE_DIR} ${OPENGL_INCLUDE_DIR} ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${TIFF_INCLUDE_DIRS})

add_executable(MID main.cpp display.cpp  rectangle.cpp parsers/nifti.cpp)

target_link_libraries(MID ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} glfw ${TIFF_LIBRARIES})