#include "meshrenderable.h"

MeshRenderable::MeshRenderable()
{

}

MeshRenderable::~MeshRenderable(){

}

void MeshRenderable::fillCoords(){

    unsigned int k;
    unsigned short n, m;
    HalfEdge* currentEdge;

    coords->clear();
    coords->squeeze();
    coords->reserve(mesh->Vertices.size());

    for (k=0; k<(unsigned int)mesh->Vertices.size(); ++k)
      coords->append(mesh->Vertices[k].coords);

    indices->clear();
    indices->squeeze();
    indices->reserve(mesh->HalfEdges.size() + mesh->Faces.size());

    for (k=0; k<(unsigned int)mesh->Faces.size(); k++) {
        n = mesh->Faces[k].val;
        if (not mesh->Faces[k].side)
            qDebug() << "side undefined";
        currentEdge = mesh->Faces[k].side;
        for (m=0; m<n; m++) {
            indices->append(currentEdge->target->index);
            currentEdge = currentEdge->next;
        }
        indices->append(maxInt);
    }
}

void MeshRenderable::fillColours(){

    unsigned int k;

    colours->clear();
    colours->squeeze();
    colours->reserve(mesh->Vertices.size());

    for (k=0; k<(unsigned int)mesh->Vertices.size(); ++k)
        colours->append(mesh->Vertices[k].colour);

}
