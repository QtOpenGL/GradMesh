#ifndef CONTROLRENDERABLE_H
#define CONTROLRENDERABLE_H

#include "meshrenderable.h"

class ControlRenderable : public MeshRenderable
{
public:
    ControlRenderable();

    void fillCoords();
};

#endif // CONTROLRENDERABLE_H
