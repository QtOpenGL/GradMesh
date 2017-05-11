#include "meshrenderable.h"

MeshRenderable::MeshRenderable()
{
    mesh = new Mesh;
}

MeshRenderable::~MeshRenderable(){
    delete mesh;
}

void MeshRenderable::fillCoords(){

    unsigned short n, m;
    HalfEdge* currentEdge;

    coords->clear();
    coords->squeeze();
    coords->reserve(mesh->Vertices.size());

    for (Vertex const &vtx : mesh->Vertices)
      coords->append(vtx.coords);

    indices->clear();
    indices->squeeze();
    indices->reserve(mesh->HalfEdges.size() + mesh->Faces.size());

    for (Face const &face : mesh->Faces) {
        n = face.val;
        currentEdge = face.side;
        for (m=0; m<n; m++) {
            indices->append(currentEdge->target->index);
            currentEdge = currentEdge->next;
        }
        indices->append(maxInt);
    }
}

void MeshRenderable::fillColours(){

    colours->clear();
    colours->squeeze();
    colours->reserve(mesh->Vertices.size());

    for (Vertex const &vtx : mesh->Vertices)
        colours->append(vtx.colour);

}
