#include "rectangle.h"

Vertex::Vertex(float xCoord, float yCoord)
{
    x = xCoord; 
    y = yCoord;
}

Vertex::Vertex()
{
    x = 0.0f;
    y = 0.0f;
}

Rectangle::Rectangle(Vertex topLeft, Vertex bottomRight)
{
    tl = topLeft;
    br = bottomRight;

    tr = Vertex(bottomRight.x, topLeft.y);
    bl = Vertex(topLeft.x, bottomRight.y);
}

Rectangle::Rectangle()
{
    
}