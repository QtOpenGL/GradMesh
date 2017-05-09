#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QOpenGLFunctions_4_1_Core>

class MainView;

class Renderable {

public:
    Renderable();
    ~Renderable();
    QVector <QVector2D> *coords;
    QVector <QVector3D> *colours;
    QVector <unsigned int> *indices;
    GLuint vao, coordsBO, colourBO, indicesBO;

    unsigned int maxInt = ((unsigned int) -1);

    void registerRenderable(MainView *ui);
    void updateRenderable(MainView *ui);
    void fillNonsense();
    bool isRegistered = false;
};



#endif
