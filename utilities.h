#include "mesh/mesh.h"

QVector2D centroid(Face *face);
float distToLine(QVector2D point, QVector2D start, QVector2D end);
void setNext(HalfEdge *next, HalfEdge *prev);
void setTwin(HalfEdge *twin1, HalfEdge *twin2);
