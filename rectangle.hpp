#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <cstdlib>

class Vertex
{
  public:
    float x, y;
    Vertex(float xCoord, float yCoord);
    Vertex();
};

class Rectangle
{
  public:
    Vertex tl, tr, bl, br;
    Rectangle(Vertex topLeft, Vertex bottomRight);
    Rectangle();
    int intersect(Rectangle rect);
};

#endif