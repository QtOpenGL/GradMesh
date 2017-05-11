#include "meshtools.h"

float max(double a, double b){
    return a > b ? a : b;
}

VertInfo vertexPoint(HalfEdge* firstEdge, Mesh* subdivMesh) {
    // This functions handels vertices with (possibly fractional) sharpness.

  unsigned short k, n;
  VertInfo sumStarPts, sumFacePts;
  VertInfo vertexPt;

  HalfEdge* currentEdge;
  Vertex* currentVertex;

  currentVertex = firstEdge->twin->target;
  n = currentVertex->val;

  currentEdge = firstEdge;

  float sharpness = 0.0;
  int incidentCreases = 0;
  currentEdge = firstEdge;
  for (int i = 0; i < n; ++i){
      if (currentEdge->sharpness > 0.0){
          sharpness += currentEdge->sharpness;
          ++incidentCreases;
          vertexPt += currentEdge->target;
      }
      currentEdge = currentEdge->prev->twin;
  }

  // At this point, vertexPt is the sum of the vertices connected to currentVertex by sharp edges.
  if (incidentCreases >= 3){ // Vertex is a corner
  // VertInfo vt(currentVertex->coords, currentVertex->colour);
      return VertInfo(currentVertex);
  }
  if (incidentCreases == 2){
      vertexPt += 6.0 * currentVertex->coords;
      vertexPt += 6.0 * currentVertex->colour;
      vertexPt /= 8.0;
      if (sharpness < 2.0){
          sharpness /= 2; // Average edge sharpness of incident edges
          vertexPt *= sharpness;
          vertexPt += currentVertex->coords *  (1-sharpness);
      }
      return vertexPt;
  }

  // Catmull-Clark (also supporting initial meshes containing n-gons)
  if (HalfEdge* boundaryEdge = vertOnBoundary(currentVertex)) {
    if (boundaryEdge->twin->target->val == 2) {
      // Interpolate corners
      return VertInfo(boundaryEdge->twin->target);
    }
    else {
        vertexPt.pos  = 1.0 * boundaryEdge->target->coords;
        vertexPt += 6.0 * boundaryEdge->twin->target->coords;
        vertexPt += 1.0 * boundaryEdge->prev->twin->target->coords;

        vertexPt.col  = 1.0 * boundaryEdge->target->colour;
        vertexPt += 6.0 * boundaryEdge->twin->target->colour;
        vertexPt += 1.0 * boundaryEdge->prev->twin->target->colour;

        vertexPt /= 8.0;
    }
  }
  else {
    for (k=0; k<n; k++) {
      sumStarPts += currentEdge->target;
      sumFacePts += subdivMesh->Vertices[currentEdge->polygon->index];
      currentEdge = currentEdge->prev->twin;
    }
    sumStarPts /= n;
    sumFacePts /= n;

    vertexPt.pos = ((n-2)*currentVertex->coords + sumStarPts.pos + sumFacePts.pos)/n;
    vertexPt.col = ((n-2)*currentVertex->colour + sumStarPts.col + sumFacePts.col)/n;
  }

  return vertexPt;
}

VertInfo ccEdgePoint(HalfEdge *currentEdge, Mesh *subdivMesh){
    VertInfo point(currentEdge->target);
    point     += currentEdge->twin->target;
    point     += subdivMesh->Vertices[currentEdge->polygon->index];
    point     += subdivMesh->Vertices[currentEdge->twin->polygon->index];

    point /= 4.0;
    return point;
}

VertInfo avEdgePoint(HalfEdge *currentEdge){
    VertInfo point(currentEdge->target);
    point  += currentEdge->twin->target;

    point /= 2.0;
    return point;
}

VertInfo edgePoint(HalfEdge* firstEdge, Mesh* subdivMesh) {
    // This is for (possibly fractional) sharpness edges, this is nicely cleaned up and factorized
  HalfEdge* currentEdge;
  currentEdge = firstEdge;

  float sharpness = currentEdge->sharpness;
  if (!currentEdge->polygon || !currentEdge->twin->polygon || sharpness > 1.0) // Edge is boundary or sharpness > 1.0
    return avEdgePoint(currentEdge);

  if (sharpness > 0.0){ // sharpness  > 0.0, lerp between smooth and sharp
      return avEdgePoint(currentEdge) * sharpness + ccEdgePoint(currentEdge, subdivMesh) * (1 - sharpness);
  }

  // Edge is smmooth
  return ccEdgePoint(currentEdge, subdivMesh);
}

VertInfo *facePoint(HalfEdge* firstEdge)
{
  unsigned short k, n;
  QVector<float> stencil;
  HalfEdge* currentEdge;

  n = firstEdge->polygon->val;

  // Bilinear, Catmull-Clark, Dual
  stencil.clear();
  stencil.fill(1.0/n, n);

  currentEdge = firstEdge;
  VertInfo *facePt = new VertInfo;

  for (k=0; k<n; k++) {
    // General approach
      *facePt += stencil[k] * currentEdge->target->coords;
      *facePt += stencil[k] * currentEdge->target->colour;
      currentEdge = currentEdge->next;
  }

  return facePt;
}

HalfEdge* vertOnBoundary(Vertex* currentVertex) {

  unsigned short n = currentVertex->val;
  int k;
  HalfEdge* currentEdge = currentVertex->out;

  for (k=0; k<n; k++) {
    if (!currentEdge->polygon) {
      return currentEdge;
    }
    currentEdge = currentEdge->prev->twin;
  }

  return nullptr;
}

// For Bilinear, Catmull-Clark and Loop
void splitHalfEdges(Mesh* inputMesh, Mesh* subdivMesh, unsigned int numHalfEdges, unsigned int numVertPts, unsigned int numFacePts) {
  unsigned int k, m;
  unsigned int vIndex;
  HalfEdge* currentEdge;

  vIndex = numFacePts + numVertPts;

  for (k=0; k<numHalfEdges; k++) {
    currentEdge = &inputMesh->HalfEdges[k];
    m = currentEdge->twin->index;

    // Target, Next, Prev, Twin, Poly, Index
    subdivMesh->HalfEdges.append(HalfEdge(nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          2*k));

    subdivMesh->HalfEdges.append(HalfEdge(nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          2*k+1));

    if (k < m) {
      subdivMesh->HalfEdges[2*k].target = &subdivMesh->Vertices[ vIndex ];
      subdivMesh->HalfEdges[2*k+1].target = &subdivMesh->Vertices[ numFacePts + currentEdge->target->index ];
      vIndex++;
    }
    else {
      subdivMesh->HalfEdges[2*k].target = subdivMesh->HalfEdges[2*m].target;
      subdivMesh->HalfEdges[2*k+1].target = &subdivMesh->Vertices[ numFacePts + currentEdge->target->index ];

      // Assign Twins
      subdivMesh->HalfEdges[2*k].twin = &subdivMesh->HalfEdges[2*m+1];
      subdivMesh->HalfEdges[2*k+1].twin = &subdivMesh->HalfEdges[2*m];
      subdivMesh->HalfEdges[2*m].twin = &subdivMesh->HalfEdges[2*k+1];
      subdivMesh->HalfEdges[2*m+1].twin = &subdivMesh->HalfEdges[2*k];

      // Boundary edges are added last when importing a mesh, so their index will always be higher than their twins.
      if (!currentEdge->polygon) {
        subdivMesh->HalfEdges[2*k].next = &subdivMesh->HalfEdges[2*k+1];
        subdivMesh->HalfEdges[2*k+1].prev = &subdivMesh->HalfEdges[2*k];

        if (currentEdge > currentEdge->next) {
          m = currentEdge->next->index;
          subdivMesh->HalfEdges[2*k+1].next = &subdivMesh->HalfEdges[2*m];
          subdivMesh->HalfEdges[2*m].prev = &subdivMesh->HalfEdges[2*k+1];
        }

        if (currentEdge > currentEdge->prev) {
          m = currentEdge->prev->index;
          subdivMesh->HalfEdges[2*k].prev = &subdivMesh->HalfEdges[2*m+1];
          subdivMesh->HalfEdges[2*m+1].next = &subdivMesh->HalfEdges[2*k];
        }
      }
    }
  }

  // Note that Next, Prev and Poly are not yet assigned at this point.

}

QVector2D faceAverage(Face* inputFace){
    int n = inputFace->val;
    HalfEdge* currentEdge = inputFace->side;
    QVector2D average = QVector2D(0.0, 0.0);
    for (int k = 0; k < n; ++k){
        average += currentEdge->target->coords;
        currentEdge = currentEdge->next;
    }
    return average / n;
}

QString vtxToString(Vertex * vtx){
    QString str = "v ";
    str.append(QString::number(float(vtx->coords[0])) + " ");
    str.append(QString::number(float(vtx->coords[1])) + " ");
    str.append(QString::number(float(vtx->colour[0])) + " ");
    str.append(QString::number(vtx->colour[1]) + " ");
    str.append(QString::number(vtx->colour[2]));
    return str;
}

QString vtxToString(VertInfo * vtx){
    QString str = "v ";
    str.append(QString::number(float(vtx->pos[0])) + " ");
    str.append(QString::number(float(vtx->pos[1])) + " ");
    str.append(QString::number(float(vtx->col[0])) + " ");
    str.append(QString::number(vtx->col[1]) + " ");
    str.append(QString::number(vtx->col[2]));
    return str;
}

QString faceToString(Face *face){
    QString str = "f ";
    HalfEdge *currentEdge;
    currentEdge = face->side;
    size_t n = face->val;
    for (size_t i = 0; i < n - 1; ++i){
        str.append(QString::number(currentEdge->target->index + 1) + " ");
        currentEdge = currentEdge->next;
    }
    str.append(QString::number(currentEdge->target->index + 1));

    return str;
}

QString faceToString(Face *face, HalfEdge *startEdge){
    QString str = "f ";
    HalfEdge *currentEdge;
    currentEdge = startEdge;
    size_t n = face->val;
    for (size_t i = 0; i < n - 1; ++i){
        str.append(QString::number(currentEdge->target->index + 1) + " ");
        currentEdge = currentEdge->next;
    }
    str.append(QString::number(currentEdge->target->index + 1));

    return str;
}

HalfEdge* isNeighbour(Face *face1, Face *face2){
    size_t n = face1->val;
    HalfEdge *currentEdge = face1->side;
    for (size_t i = 0; i < n; ++i){
        if (currentEdge->twin->polygon == face2)
            return currentEdge;
        currentEdge = currentEdge->next;
    }
    return nullptr;
}

