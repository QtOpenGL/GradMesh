#ifndef VERTINFO
#define VERTINFO

#include <QVector2D>
#include <QVector3D>

class Vertex;

struct VertInfo{

    QVector2D pos;
    QVector3D col;

    VertInfo();
    VertInfo(Vertex *vert);
    VertInfo& operator+=(const VertInfo& rhs);
    VertInfo& operator+=(const Vertex& rhs);
    VertInfo& operator+=(const QVector2D& rhs);
    VertInfo& operator+=(const QVector3D& rhs);
    VertInfo& operator/=(const float& rhs);
    VertInfo& operator*=(const float& rhs);

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

