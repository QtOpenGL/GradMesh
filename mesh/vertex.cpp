#include "vertex.h"

Vertex::Vertex() {
  qDebug() << "Default Vertex Constructor";
  coords = QVector2D();
  out = nullptr;
  val = 0;
  index = 0;
  sharpness = 0;
  colour = QVector3D();
}

Vertex::Vertex(QVector2D const &vcoords, HalfEdge* vout, unsigned short vval, unsigned int vindex, float vsharpness, QVector3D const vcolour)
    :
  coords(vcoords),
  out(vout),
  val(vval),
  index(vindex),
  sharpness(vsharpness),
  colour(vcolour)
{}

Vertex::Vertex(VertInfo const *info, HalfEdge* vout, unsigned short vval, unsigned int vindex, float vsharpness)
    :
  coords(info->pos),
  out(vout),
  val(vval),
  index(vindex),
  sharpness(vsharpness),
  colour(info->col)
{
    delete info;
}
