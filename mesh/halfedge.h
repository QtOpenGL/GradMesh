#ifndef HALFEDGE
#define HALFEDGE

#include <QVector2D>

// Forward declarations
class Vertex;
class Face;

class HalfEdge {

public:
  Vertex* target;
  HalfEdge* next;
  HalfEdge* prev;
  HalfEdge* twin;
  Face* polygon;
  unsigned int index;
  float sharpness;
  QVector2D colGrad;

  // Inline constructors

  HalfEdge() {
      qDebug() << "defult edge";
    target = nullptr;
    next = nullptr;
    prev = nullptr;
    twin = nullptr;
    polygon = nullptr;
    index = 0;
    sharpness = 0;
    colGrad = QVector2D(0.0,0.0);
  }

  HalfEdge(Vertex* htarget, HalfEdge* hnext, HalfEdge* hprev, HalfEdge* htwin, Face* hpolygon, unsigned int hindex, float hsharpness=0) {
    target = htarget;
    next = hnext;
    prev = hprev;
    twin = htwin;
    polygon = hpolygon;
    index = hindex;
    sharpness = hsharpness;
  }

  HalfEdge(Vertex* htarget, HalfEdge* hnext, HalfEdge* hprev, HalfEdge* htwin, Face* hpolygon, unsigned int hindex, float hsharpness, QVector2D const &hcolGrad) {
    target = htarget;
    next = hnext;
    prev = hprev;
    twin = htwin;
    polygon = hpolygon;
    index = hindex;
    sharpness = hsharpness;
    colGrad = hcolGrad;
  }

  inline QVector2D start() const{
      return twin->target->coords;
  }

  inline QVector2D end() const{
      return target->coords;
  }

  inline float length() const{
      return (start() - end()).length();
  }
};

#endif // HALFEDGE
