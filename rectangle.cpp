#include "rectangle.hpp"

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

//returns 1 if collision detected, returns 0 if they do not collide
int Rectangle::intersect(Rectangle rect)
{
    if (tl.x < rect.br.x && br.x > rect.tl.x &&
        tl.y > rect.br.y && br.y < rect.tl.y)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}