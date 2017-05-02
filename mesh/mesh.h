#ifndef MESH_H
#define MESH_H

#include <QVector>

#include "vertex.h"
#include "face.h"
#include "halfedge.h"
#include "objfile.h"

class Mesh {

public:
    Mesh();
    Mesh(OBJFile* loadedOBJFile);
    ~Mesh();

    QVector<Vertex> Vertices;
    QVector<Face> Faces;
    QVector<HalfEdge> HalfEdges;

    void setTwins(size_t numHalfEdges, size_t indexH);

    QVector<QVector<size_t> > PotentialTwins;

    void copy(Mesh *mesh);

    void dispVertInfo(unsigned short vertIndex);
    void dispHalfEdgeInfo(unsigned short edgeIndex);
    void dispFaceInfo(unsigned short faceIndex);

};

#endif // MESH_H
