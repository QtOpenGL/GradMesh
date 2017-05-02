#include "utilities.h"

QVector2D centroid(Face *face){
    QVector2D center = {0.0, 0.0};
    HalfEdge *currentEdge = face->side;
    size_t n = face->val;
    for (size_t i = 0; i < n; ++i){
        center += currentEdge->target->coords;
        currentEdge = currentEdge->next;
    }
    return center / n;
}

float distToLine(QVector2D point, QVector2D start, QVector2D end){
    point = point - start;
    end   = end - start;

    float d = QVector2D::dotProduct(end, point);
    if (d < 0 || d > end.lengthSquared())
        return 10000.0f;

    end.normalize();
    point = point - d * end;
    return point.length();
}

void setNext(HalfEdge *next, HalfEdge *prev){
    next->prev = prev;
    prev->next = next;
}

void setTwin(HalfEdge *twin1, HalfEdge *twin2){
    twin1->twin = twin2;
    twin2->twin = twin1;
}
