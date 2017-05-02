#ifndef VERTEX
#define VERTEX

#include <QVector2D>
#include <QVector3D>
#include <QDebug>

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

  // Inline constructors
  Vertex() {
    // qDebug() << "Default Vertex Constructor";
    coords = QVector2D();
    out = nullptr;
    val = 0;
    index = 0;
    sharpness = 0;
    colour = QVector3D();
  }

  Vertex(QVector2D &vcoords, HalfEdge* vout, unsigned short vval, unsigned int vindex, float vsharpness = 0, QVector3D vcolour = QVector3D(1.0, 1.0, 0.0)) {
    //qDebug() << "QVector3D Vertex Constructor";
    coords = vcoords;
    out = vout;
    val = vval;
    index = vindex;
    sharpness = vsharpness;
    colour = vcolour;
  }
};

#endif // VERTEX
