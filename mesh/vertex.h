#ifndef VERTEX
#define VERTEX

#include <QVector2D>
#include <QVector3D>
#include <QDebug>
#include "vertinfo.h"

// Forward declaration
class HalfEdge;

class Vertex {
public:
  QVector2D coords;
  HalfEdge* out;
  unsigned short val;
  unsigned int index;
  unsigned short sharpness;
  QVector3D colour;

  Vertex();
  Vertex(QVector2D const &vcoords, HalfEdge* vout, unsigned short vval, unsigned int vindex,
         float vsharpness = 0, QVector3D const &vcolour = QVector3D(1.0, 1.0, 0.0));
  Vertex(VertInfo const *info, HalfEdge* vout, unsigned short vval, unsigned int vindex, float vsharpness = 0);
};


#endif // VERTEX
