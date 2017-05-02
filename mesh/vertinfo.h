#ifndef VERTINFO
#define VERTINFO

#include "mesh/vertex.h"
#include "mesh/face.h"
#include "mesh/halfedge.h"

struct VertInfo{

    QVector2D pos;
    QVector3D col;

    VertInfo()
        :
          pos(0.0, 0.0),
          col(0.0, 0.0, 0.0)

    {}

    VertInfo(Vertex *vert){
        pos = vert->coords;
        col = vert->colour;
    }

    VertInfo& operator+=(const VertInfo& rhs)
    {
        pos += rhs.pos;
        col += rhs.col;
        return *this;
    }

    VertInfo& operator+=(const Vertex& rhs)
    {
        pos += rhs.coords;
        col += rhs.colour;
        return *this;
    }

    VertInfo& operator+=(const QVector2D& rhs)
    {
        pos += rhs;
        return *this;
    }

    VertInfo& operator+=(const QVector3D& rhs)
    {
        col += rhs;
        return *this;
    }

    VertInfo& operator/=(const float& rhs)
    {
        pos /= rhs;
        col /= rhs;
        return *this;
    }

    VertInfo& operator*=(const float& rhs)
    {
        pos *= rhs;
        col *= rhs;
        return *this;
    }

};

inline VertInfo operator+(VertInfo lhs, const VertInfo& rhs)
{
  lhs += rhs;
  return lhs;
}

inline VertInfo operator*(VertInfo lhs, const float& rhs)
{
  lhs *= rhs;
  return lhs;
}

#endif // VERTINFO

