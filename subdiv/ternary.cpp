#include "../mesh/mesh.h"
#include "../utilities.h"

void ternaryStep(Mesh *inputMesh, Mesh *outputMesh){
    size_t k, l;
    size_t vertCount = inputMesh->Vertices.size();
    size_t edgeCount = inputMesh->HalfEdges.size();
    size_t faceCount = inputMesh->Faces.size();

    size_t sumFaceVal = 0;
    size_t newFaceCount = 0;

    for (k = 0; k < faceCount; ++k){
        sumFaceVal += inputMesh->Faces[k].val;
        newFaceCount += 1 + 2 * inputMesh->Faces[k].val;
    }

    size_t newVertCount = vertCount + edgeCount + sumFaceVal;
    outputMesh->Vertices.clear();
    outputMesh->Vertices.reserve(newVertCount);

    for (k = 0; k < vertCount; ++k)
        outputMesh->Vertices.append(inputMesh->Vertices[k]); // out pointer is nonsense at this point

    HalfEdge *currentEdge;
    HalfEdge *twinEdge;
    QVector2D newPosition;
    for (k = 0; k < edgeCount; ++k){
        currentEdge = &inputMesh->HalfEdges[k];
        twinEdge = currentEdge->twin;
        newPosition = twinEdge->target->coords + currentEdge->colGrad;
        outputMesh->Vertices.append(Vertex(
                                      newPosition,
                                      nullptr,
                                      4,
                                      vertCount + k,
                                      0,
                                      twinEdge->target->colour)
                                    );
    }

    float d1, d2, N1, N2, V01, V02;
    QVector2D V0, V1, V2, D1, D2, C;      // V1 ---- V0 ---- V2

//    Vertex(QVector2D &vcoords, HalfEdge* vout, unsigned short vval, unsigned int vindex, float vsharpness = 0, QVector3D vcolour = QVector3D(1.0, 1.0, 0.0)) {
    qDebug() << "dfdsfdfdfdfd";

    size_t vertIndex = vertCount + edgeCount;
    for (k = 0; k < faceCount; ++k){
        currentEdge = inputMesh->Faces[k].side;
        C = centroid(&inputMesh->Faces[k]);
        for (l = 0; l < inputMesh->Faces[k].val; ++l){
            V1 = currentEdge->prev->target->coords;
            V0 = currentEdge->target->coords;
            V2 = currentEdge->next->target->coords;
            D1 = currentEdge->twin->colGrad;
            D2 = currentEdge->next->colGrad;
            N1 = D1.length();
            N2 = D2.length();
            if (N1 == 0.0 || N2 == 0.0)
                qDebug() << "Zero length colour gradient";

            V01 = QVector2D(V0 - V1).length();
            V02 = QVector2D(V0 - V2).length();

            d1 = 2 * N1 / V01;
            d2 = 2 * N2 / V02;

            newPosition =
                (1 - d1) * (1-d2) * V0 +
                d1 * d2 * C +
                d1 * (1 - d2) * (V0 + V01 * D1 / (2 * N1)) +
                d2 * (1 - d1) * (V0 + V02 * D2 / (2 * N2));

            outputMesh->Vertices.append(Vertex(
                                            newPosition,
                                            nullptr,
                                            4,
                                            vertIndex,
                                            0,
                                            currentEdge->target->colour
                                            ));
            currentEdge = currentEdge->next;
            ++vertIndex;
          }
    }

    size_t newEdgeCount = 3 * edgeCount + 6 * sumFaceVal;
    outputMesh->HalfEdges.clear();
    outputMesh->HalfEdges.reserve(newEdgeCount);

    for (k = 0; k < edgeCount; ++k){
        currentEdge = &inputMesh->HalfEdges[k];
        outputMesh->HalfEdges.append(HalfEdge(&outputMesh->Vertices[vertCount + currentEdge->index]      , nullptr, nullptr, nullptr, nullptr, 3 * k));
        outputMesh->HalfEdges.append(HalfEdge(&outputMesh->Vertices[vertCount + currentEdge->twin->index], nullptr, nullptr, nullptr, nullptr, 3 * k + 1));
        outputMesh->HalfEdges.append(HalfEdge(&outputMesh->Vertices[currentEdge->target->index]          , nullptr, nullptr, nullptr, nullptr, 3 * k + 2));

        // /|    /|
        //  |     |  2
        //  |     |
        //  |    /|
        //  | ->  |  1
        //  |     |
        //  |    /|
        //  |     |  0
        //  |     |
    }

    size_t twinIndex;
    for (k = 0; k < edgeCount; ++k){
        currentEdge = &inputMesh->HalfEdges[k];
        twinIndex = currentEdge->twin->index;
        outputMesh->HalfEdges[3 * k].twin     = &outputMesh->HalfEdges[3 * twinIndex + 2];
        outputMesh->HalfEdges[3 * k + 1].twin = &outputMesh->HalfEdges[3 * twinIndex + 1];
        outputMesh->HalfEdges[3 * k + 2].twin = &outputMesh->HalfEdges[3 * twinIndex];

        outputMesh->HalfEdges[3 * k].prev     = &outputMesh->HalfEdges[3 * currentEdge->prev->index + 2];
        outputMesh->HalfEdges[3 * k + 2].next = &outputMesh->HalfEdges[3 * currentEdge->next->index];

        if (not currentEdge->polygon){
            setNext(&outputMesh->HalfEdges[3 * k + 1], &outputMesh->HalfEdges[3 * k]);
            setNext(&outputMesh->HalfEdges[3 * k + 2], &outputMesh->HalfEdges[3 * k + 1]);
        }

        twinEdge = currentEdge->twin;
        if (twinEdge->index < currentEdge->index){
            outputMesh->Vertices[twinEdge->target->index].out = &outputMesh->HalfEdges[3 * k];
            outputMesh->Vertices[vertCount + currentEdge->index].out = &outputMesh->HalfEdges[3 * k + 1];
            outputMesh->Vertices[vertCount + twinIndex].out = &outputMesh->HalfEdges[3 * k + 2];
        }
    }

    Face *currentFace;
    size_t n;
    size_t edgeIndex = 3 * edgeCount;
    vertIndex = vertCount + edgeCount;
    size_t currentIndex;
    size_t nextIndex;
    size_t edgesAdded;
    size_t faceIndex = 0;
    outputMesh->Faces.clear();
    outputMesh->Faces.reserve(newFaceCount);
    for (k = 0; k < faceCount; ++k){
        currentFace = &inputMesh->Faces[k];
        n = currentFace->val;
        currentEdge = currentFace->side;

        for (l = 0; l < n; ++l) {
            currentIndex = currentEdge->index;
            nextIndex = currentEdge->next->index;
            twinIndex = currentEdge->twin->index;

            outputMesh->HalfEdges.append(HalfEdge(&outputMesh->Vertices[vertIndex]            , nullptr, nullptr, nullptr, nullptr, edgeIndex    ));
            outputMesh->HalfEdges.append(HalfEdge(&outputMesh->Vertices[vertIndex]            , nullptr, nullptr, nullptr, nullptr, edgeIndex + 1));
            outputMesh->HalfEdges.append(HalfEdge(&outputMesh->Vertices[vertCount + nextIndex], nullptr, nullptr, nullptr, nullptr, edgeIndex + 2));
            outputMesh->HalfEdges.append(HalfEdge(&outputMesh->Vertices[vertIndex]            , nullptr, nullptr, nullptr, nullptr, edgeIndex + 3));
            outputMesh->HalfEdges.append(HalfEdge(&outputMesh->Vertices[vertCount + twinIndex], nullptr, nullptr, nullptr, nullptr, edgeIndex + 4));
            outputMesh->HalfEdges.append(HalfEdge(&outputMesh->Vertices[vertIndex]            , nullptr, nullptr, nullptr, nullptr, edgeIndex + 5));

            setNext( &outputMesh->HalfEdges[edgeIndex + 2]       , &outputMesh->HalfEdges[edgeIndex + 1]       );
            setNext( &outputMesh->HalfEdges[edgeIndex + 3]       , &outputMesh->HalfEdges[3 * nextIndex]       );
            setNext( &outputMesh->HalfEdges[edgeIndex + 4]       , &outputMesh->HalfEdges[edgeIndex + 3]       );
            setNext( &outputMesh->HalfEdges[edgeIndex + 5]       , &outputMesh->HalfEdges[3 * currentIndex + 1]);
            setNext( &outputMesh->HalfEdges[3 * nextIndex + 1]   , &outputMesh->HalfEdges[edgeIndex + 2]       );
            setNext( &outputMesh->HalfEdges[3 * currentIndex + 2], &outputMesh->HalfEdges[edgeIndex + 4]       );

            setTwin( &outputMesh->HalfEdges[edgeIndex + 2], &outputMesh->HalfEdges[edgeIndex + 3] );
            setTwin( &outputMesh->HalfEdges[edgeIndex + 4], &outputMesh->HalfEdges[edgeIndex + 5] );

            outputMesh->Vertices[vertIndex].out = &outputMesh->HalfEdges[edgeIndex + 2];

            outputMesh->Faces.append(Face(&outputMesh->HalfEdges[edgeIndex + 1], 4, faceIndex));
            outputMesh->Faces.append(Face(&outputMesh->HalfEdges[edgeIndex + 4], 4, faceIndex + 1));

            faceIndex += 2;
            ++vertIndex;
            edgeIndex += 6;
            currentEdge = currentEdge->next;
        }

        edgesAdded = 6 * n;
        edgeIndex -= edgesAdded;

        outputMesh->Faces.append(Face(&outputMesh->HalfEdges[edgeIndex], n, faceIndex));
        ++faceIndex;

        setTwin( &outputMesh->HalfEdges[edgeIndex], &outputMesh->HalfEdges[edgeIndex + 6 * (n - 1) + 1] );
        setNext( &outputMesh->HalfEdges[edgeIndex + 6 * (n - 1) + 1], &outputMesh->HalfEdges[edgeIndex + 5] );
        setNext( &outputMesh->HalfEdges[edgeIndex], &outputMesh->HalfEdges[edgeIndex + 6 * (n - 1)] );

        for (l = 0; l < n - 1; ++l){
            setTwin( &outputMesh->HalfEdges[edgeIndex + 1], &outputMesh->HalfEdges[edgeIndex + 6] );
            setNext( &outputMesh->HalfEdges[edgeIndex + 1], &outputMesh->HalfEdges[edgeIndex + 11] );
            setNext( &outputMesh->HalfEdges[edgeIndex + 6], &outputMesh->HalfEdges[edgeIndex] );
            edgeIndex += 6;
        }
        edgeIndex += 6;
    }


    for (size_t i = 0; i < newFaceCount; ++i){
        n = outputMesh->Faces[i].val;
        currentEdge = outputMesh->Faces[i].side;
        for (size_t j = 0; j < n; ++j){
            currentEdge->polygon = &outputMesh->Faces[i];
            currentEdge = currentEdge->next;
        }
    }

//    qDebug() << " * Completed the ternary step!";
//    qDebug() << "   # Vertices:" << outputMesh->Vertices.size();
//    qDebug() << "   # HalfEdges:" << outputMesh->HalfEdges.size();
//    qDebug() << "   # Faces:" << outputMesh->Faces.size();


}
