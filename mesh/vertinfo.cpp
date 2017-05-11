#include "vertinfo.h"
#include "vertex.h"

VertInfo::VertInfo()
    :
      pos(0.0, 0.0),
      col(0.0, 0.0, 0.0)

{}

VertInfo::VertInfo(Vertex *vert){
    pos = vert->coords;
    col = vert->colour;
}

VertInfo& VertInfo::operator+=(const VertInfo& rhs)
{
    pos += rhs.pos;
    col += rhs.col;
    return *this;
}

VertInfo& VertInfo::operator+=(const Vertex& rhs)
{
    pos += rhs.coords;
    col += rhs.colour;
    return *this;
}

VertInfo& VertInfo::operator+=(const QVector2D& rhs)
{
    pos += rhs;
    return *this;
}

VertInfo& VertInfo::operator+=(const QVector3D& rhs)
{
    col += rhs;
    return *this;
}

VertInfo& VertInfo::operator/=(const float& rhs)
{
    pos /= rhs;
    col /= rhs;
    return *this;
}

VertInfo& VertInfo::operator*=(const float& rhs)
{
    pos *= rhs;
    col *= rhs;
    return *this;
}


