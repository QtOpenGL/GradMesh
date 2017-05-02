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
    target = nullptr;
    next = nullptr;
    prev = nullptr;
    twin = nullptr;
    polygon = nullptr;
    index = 0;
    sharpness = 0;
    colGrad = QVector2D(0.0,0.0);
  }

  HalfEdge(Vertex* htarget, HalfEdge* hnext, HalfEdge* hprev, HalfEdge* htwin, Face* hpolygon, unsigned int hindex, float hsharpness=0, QVector2D hcolGrad = QVector2D(0.0, 0.0)) {
    target = htarget;
    next = hnext;
    prev = hprev;
    twin = htwin;
    polygon = hpolygon;
    index = hindex;
    sharpness = hsharpness;
    colGrad = hcolGrad;
  }

  QVector2D start(){
      return twin->target->coords;
  }

  QVector2D end(){
      return target->coords;
  }

};

#endif // HALFEDGE
