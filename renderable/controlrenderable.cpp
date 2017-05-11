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

    Vertex *currentVertex;
    HalfEdge *currentEdge;
    unsigned int index = 0;
    size_t n;
    for (size_t k=0; k < (size_t)mesh->Vertices.size(); ++k){
        currentVertex = &mesh->Vertices[k];
        coords->append(currentVertex->coords);
        colours->append(currentVertex->colour);
        indices->append(index);

        n = currentVertex->val;
        currentEdge = currentVertex->out;
        for (size_t l = 0; l < n; ++l){
            coords->append(currentVertex->coords + currentEdge->colGrad);
            indices->append(index);
            colours->append(QVector3D({1.0, 1.0, 1.0}));

            currentEdge = currentEdge->prev->twin;
            index++;
        }
    }
}
