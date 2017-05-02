#ifndef SUBDIV
#define SUBDIV

#include "../mesh/mesh.h"

void ternaryStep(Mesh *inputMesh, Mesh *outputMesh);
void subdivideCatmullClark(Mesh* inputMesh, Mesh* subdivMesh);
OBJFile ccSingleFace(Mesh *inputMesh, size_t faceToBeRefined);
OBJFile makeNGON(size_t n);
void divideEdge(HalfEdge *edge, Mesh *mesh);
#endif // SUBDIV

