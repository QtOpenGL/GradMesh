#include "controlrenderable.h"
#include "../mesh/mesh.h"

ControlRenderable::ControlRenderable()
{

}

void ControlRenderable::fillCoords(){
    coords->clear();
    coords->squeeze();
    colours->clear();
    colours->squeeze();
    indices->clear();
    indices->squeeze();

    HalfEdge *currentEdge;
    unsigned int index = 0;
    size_t n;
    QVector2D vtxCoords;
    for (Vertex const &vtx : mesh->Vertices){
        vtxCoords = vtx.coords;
        coords->append(vtxCoords);
        colours->append(vtx.colour);
        indices->append(index);

        n = vtx.val;
        currentEdge = vtx.out;
        for (size_t l = 0; l < n; ++l){
            coords->append(vtxCoords + currentEdge->colGrad);
            indices->append(index);
            colours->append(QVector3D({1.0, 1.0, 1.0}));

            currentEdge = currentEdge->prev->twin;
            index++;
        }
    }
}
