#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWidget>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>

#include <QVector2D>
#include <QMouseEvent>
#include "mousehandler.h"

class MainView : public QOpenGLWidget, public QOpenGLFunctions_4_1_Core {

    Q_OBJECT

public:
    MainView(QWidget *parent = 0);
    ~MainView();

    unsigned int maxInt = ((unsigned int) -1);
    void updateBuffers();
    void paintGL();

    bool showTernaryMesh = false;
    bool showControlNet = true;
    bool showGradients = true;
    bool showColourSurface = true;
    bool showSkeleton = false;

    QOpenGLShaderProgram* mainShaderProg,* blackShaderProg,* whiteShaderProg,* greyShaderProg;

    DISPLAY disp = DISPLAY::ORIGINAL;

    QVector <Renderables *> *rndrbles;
    MouseHandler *mouseHandler;

    float quadrantEntries[5][3] = {
        {0.0f, 0.0f, 1.0f},
        {0.5f, -0.5f, 0.5f},
        {-0.5f, -0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        {-0.5f, 0.5f, 0.5f}
    };
    void createBuffer(size_t index);

protected:
    void initializeGL();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:

    void bindShader(QOpenGLShaderProgram *program, size_t qdrnt);

    int uniDx[3];
    int uniDy[3];
    int uniScale[3];

    void registerRenderable(Renderable *obj);
    void updateRenderable(Renderable *obj);
    void renderRenderable(Renderable *obj, QOpenGLShaderProgram*shaderProg, GLenum mode, size_t qdrnt);
    void renderRenderablePoints(Renderable *obj, size_t startIndex, size_t endIndex, size_t qdrnt);

    QOpenGLDebugLogger* debugLogger;

    void createShaderPrograms();
    void createBuffers();
    void resizeGL(int newWidth, int newHeight);

private slots:
    void onMessageLogged( QOpenGLDebugMessage Message );

};

#endif // MAINVIEW_H
