#include "subdiv.h"
#include "meshtools.h"
#include "utilities.h"
#include <QString>
#include <assert.h>
#include <QTextStream>
#include <QFile>

using namespace std;


OBJFile ccSingleFace(Mesh *inputMesh, size_t faceToBeRefined){

    size_t counter = 0;
    QString str;
    for (int i = 0; i < inputMesh->Vertices.size(); ++i){
        str.append(vtxToString(&inputMesh->Vertices[i]));
        str += QString("\n");
        ++counter;
    }
    str.append("s off\n");
    for (int i = 0; i < inputMesh->Faces.size(); ++i){
        if (not isNeighbour(&inputMesh->Faces[i], &inputMesh->Faces[faceToBeRefined]) && size_t(i) != faceToBeRefined){
            str.append(faceToString(&inputMesh->Faces[i]));
            str += QString("\n");
            ++counter;
        }
    }
    size_t numVerts = inputMesh->Vertices.size();
    size_t numOldVerts = numVerts;
    VertInfo vertInfo;
    size_t n = inputMesh->Faces[faceToBeRefined].val;
    Face *currentFace = &inputMesh->Faces[faceToBeRefined];
    HalfEdge *currentEdge = currentFace->side;

    for (size_t i = 0; i < n; ++i){
        str.append(vtxToString(avEdgePoint(currentEdge)));
        str += QString("\n");
        ++numVerts;

        if (currentEdge->twin->polygon){
            str.append(faceToString(currentEdge->twin->polygon, currentEdge->twin));
            str.append(" " + QString::number(numVerts));
            str += QString("\n");
        }

        currentEdge = currentEdge->next;
    }

    currentEdge = inputMesh->Faces[faceToBeRefined].side;
    str.append(vtxToString(facePoint(currentEdge)));
    str.append("\n");

    currentFace = &inputMesh->Faces[faceToBeRefined];
    currentEdge = currentFace->side;


    for (size_t i = 0; i < n; ++i){
        str.append("f ");
        str.append(QString::number(numOldVerts + i + 1) + " ");
        str.append(QString::number(currentEdge->target->index + 1) + " ");
        str.append(QString::number(numOldVerts + ((i + 1) % n) + 1) + " ");
        str.append(QString::number(numVerts + 1));
        str += QString("\n");
        currentEdge = currentEdge->next;
    }
    QFile caFile("output.txt");
    caFile.open(QIODevice::WriteOnly | QIODevice::Text);

    assert(caFile.isOpen());
    QTextStream outStream(&caFile);
    outStream << str;
    caFile.close();

    OBJFile obj("output.txt");
    return obj;
}
























































