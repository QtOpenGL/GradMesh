#ifndef MESHTOOLS_H
#define MESHTOOLS_H

#include "mesh/mesh.h"
#include <QVector2D>
#include <QVector3D>
#include "mesh/vertinfo.h"

void subdivideCatmullClark(Mesh* inputMesh, Mesh *subdivMesh);

VertInfo vertexPoint(HalfEdge* firstEdge, Mesh *newMesh);
VertInfo edgePoint(HalfEdge* firstEdge, Mesh* newMesh);
VertInfo facePoint(HalfEdge* firstEdge);
VertInfo avEdgePoint(HalfEdge *currentEdge);
HalfEdge* vertOnBoundary(Vertex* currentVertex);
QString vtxToString(Vertex * vtx);
QString vtxToString(VertInfo * vtx);
QString faceToString(Face *face);
QString faceToString(Face *face, HalfEdge *startEdge);
HalfEdge* isNeighbour(Face *face1, Face *face2);

void splitHalfEdges(Mesh* inputMesh, Mesh* subdivMesh, unsigned int numHalfEdges, unsigned int numVertPts, unsigned int numFacePts);

#endif // MESHTOOLS_H
