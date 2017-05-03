#include "meshtools.h"

int allSameFaceIndex(Face *face){
    HalfEdge *currentEdge = face->side;
    int index = currentEdge->target->originalFaceIndex;
    if (index == -1)
        return -1;
    for (int i = 0; i < face->val; ++i){
        if (currentEdge->target->originalFaceIndex != index)
            return -1;
        currentEdge = currentEdge->next;
    }
    return index;
}


void subdivideCatmullClark(Mesh* inputMesh, Mesh* subdivMesh) {

  unsigned int numVerts, numHalfEdges, numFaces, sumFaceValences;
  unsigned int k, m, s, t;
  unsigned int vIndex, hIndex, fIndex;
  unsigned short n;
  HalfEdge* currentEdge;

  subdivMesh->HalfEdges.clear();
  subdivMesh->HalfEdges.squeeze();
  subdivMesh->Vertices.clear();
  subdivMesh->Vertices.squeeze();
  subdivMesh->Faces.clear();
  subdivMesh->Faces.squeeze();

//  qDebug() << ":: Creating new Catmull-Clark mesh";

  numVerts = inputMesh->Vertices.size();
  numHalfEdges = inputMesh->HalfEdges.size();
  numFaces = inputMesh->Faces.size();

  // Reserve memory
  subdivMesh->Vertices.reserve(numFaces + numVerts + numHalfEdges/2);

  sumFaceValences = 0;
  for (k=0; k<numFaces; k++)
    sumFaceValences += inputMesh->Faces[k].val;


  subdivMesh->HalfEdges.reserve(2*numHalfEdges + 2*sumFaceValences);
  subdivMesh->Faces.reserve(sumFaceValences);

  // Create face points
  VertInfo newVert;
  for (k=0; k<numFaces; k++) {
    n = inputMesh->Faces[k].val;
    newVert = facePoint(inputMesh->Faces[k].side);

//    int fIndex = allSameFaceIndex(&inputMesh->Faces[k]);

    // Coords (x,y), Out, Valence, Index
    subdivMesh->Vertices.append( Vertex(newVert.pos,
                                        nullptr,
                                        n,
                                        k,
                                        0,
                                        newVert.col
                                        ) );
  }

//  qDebug() << " * Created face points";

  vIndex = numFaces;
  // Create vertex points
  for (k=0; k<numVerts; k++) {
      n = inputMesh->Vertices[k].val;
      newVert = vertexPoint(inputMesh->Vertices[k].out, subdivMesh);
      subdivMesh->Vertices.append( Vertex(newVert.pos,
                                          nullptr,
                                          n,
                                          vIndex,
                                          0,
                                          newVert.col
                                          ) );
      vIndex++;
    }

//  qDebug() << " * Created vertex points";

  // Create edge points
  for (k=0; k<numHalfEdges; k++) {
    currentEdge = &inputMesh->HalfEdges[k];

    if (k < currentEdge->twin->index) {
      m = (!currentEdge->polygon || !currentEdge->twin->polygon) ? 3 : 4;
      newVert = edgePoint(currentEdge, subdivMesh);
      // Coords (x,y), Out, Valence, Index
      subdivMesh->Vertices.append( Vertex(newVert.pos,
                                          nullptr,
                                          m,
                                          vIndex,
                                          0,
                                          newVert.col) );
      vIndex++;
    }
  }

//  qDebug() << " * Created edge points";

  // Split halfedges
  splitHalfEdges(inputMesh, subdivMesh, numHalfEdges, numVerts, numFaces);

//  qDebug() << " * Split halfedges";

  hIndex = 2*numHalfEdges;
  fIndex = 0;

  // Create faces and remaining halfedges
  for (k=0; k<numFaces; k++) {
    currentEdge = inputMesh->Faces[k].side;
    if (not currentEdge)
        qDebug() << "side missing in input mesh ";

    n = inputMesh->Faces[k].val;

    for (m=0; m<n; m++) {

      s = currentEdge->prev->index;
      t = currentEdge->index;

      // Side, Val, Index
      subdivMesh->Faces.append(Face(nullptr,
                                    4,
                                    fIndex));

      subdivMesh->Faces[fIndex].side = &subdivMesh->HalfEdges[ 2*t ];

      // Target, Next, Prev, Twin, Poly, Index
      subdivMesh->HalfEdges.append(HalfEdge( &subdivMesh->Vertices[k],
                                             nullptr,
                                             &subdivMesh->HalfEdges[ 2*t ],
                                   nullptr,
                                   &subdivMesh->Faces[fIndex],
                                   hIndex ));

      subdivMesh->HalfEdges.append(HalfEdge( nullptr,
                                             &subdivMesh->HalfEdges[2*s+1],
                                   &subdivMesh->HalfEdges[hIndex],
                                   nullptr,
                                   &subdivMesh->Faces[fIndex],
                                   hIndex+1 ));

      subdivMesh->HalfEdges[hIndex].next = &subdivMesh->HalfEdges[hIndex+1];
      subdivMesh->HalfEdges[hIndex+1].target = subdivMesh->HalfEdges[hIndex+1].next->twin->target;

      subdivMesh->HalfEdges[2*s+1].next = &subdivMesh->HalfEdges[2*t];
      subdivMesh->HalfEdges[2*s+1].prev = &subdivMesh->HalfEdges[hIndex+1];
      subdivMesh->HalfEdges[2*s+1].polygon = &subdivMesh->Faces[fIndex];

      subdivMesh->HalfEdges[2*t].next = &subdivMesh->HalfEdges[hIndex];
      subdivMesh->HalfEdges[2*t].prev = &subdivMesh->HalfEdges[2*s+1];
      subdivMesh->HalfEdges[2*t].polygon = &subdivMesh->Faces[fIndex];

      if (m > 0) {
        // Twins
        subdivMesh->HalfEdges[hIndex+1].twin = &subdivMesh->HalfEdges[hIndex-2];
        subdivMesh->HalfEdges[hIndex-2].twin = &subdivMesh->HalfEdges[hIndex+1];
      }

      // For edge points
      subdivMesh->HalfEdges[2*t].target->out = &subdivMesh->HalfEdges[hIndex];

      hIndex += 2;
      fIndex++;
      currentEdge = currentEdge->next;
    }

    subdivMesh->HalfEdges[hIndex-2*n+1].twin = &subdivMesh->HalfEdges[hIndex-2];
    subdivMesh->HalfEdges[hIndex-2].twin = &subdivMesh->HalfEdges[hIndex-2*n+1];

    // For face points
    subdivMesh->Vertices[k].out = &subdivMesh->HalfEdges[hIndex-1];

  }

//  qDebug() << " * Created faces and remaining halfedges";

  // For vertex points
  for (k=0; k<numVerts; k++) {
    subdivMesh->Vertices[numFaces + k].out = &subdivMesh->HalfEdges[ 2*inputMesh->Vertices[k].out->index ];
  }

//  qDebug() << " * Completed the Catmull-Clark step!";
//  qDebug() << "   # Vertices:" << subdivMesh->Vertices.size();
//  qDebug() << "   # HalfEdges:" << subdivMesh->HalfEdges.size();
//  qDebug() << "   # Faces:" << subdivMesh->Faces.size();

  for (int i = 0; i < inputMesh->HalfEdges.size(); ++i){
      subdivMesh->HalfEdges[2 * i].sharpness = qMax(inputMesh->HalfEdges[i].sharpness - 1.0, 0.0);
      subdivMesh->HalfEdges[2 * i + 1].sharpness = qMax(inputMesh->HalfEdges[i].sharpness - 1.0, 0.0);
  }

}
