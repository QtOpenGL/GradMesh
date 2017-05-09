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
    bool showRefine = false;

    QOpenGLShaderProgram* mainShaderProg,* blackShaderProg,* whiteShaderProg,* greyShaderProg;

    Renderables *rndrbles;
    MouseHandler *mouseHandler;
    void createBuffer();
    size_t ref_level = 0;

protected:
    void initializeGL();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    void bindShader(QOpenGLShaderProgram *program);

    void registerRenderable(Renderable *obj);
    void updateRenderable(Renderable *obj);
    void renderRenderable(Renderable *obj, QOpenGLShaderProgram*shaderProg, GLenum mode);
    void renderRenderablePoints(Renderable *obj, size_t startIndex, size_t count);

    QOpenGLDebugLogger* debugLogger;

    void createShaderPrograms();
    void createBuffers();
    void resizeGL(int newWidth, int newHeight);

private slots:
    void onMessageLogged( QOpenGLDebugMessage Message );

};

#endif // MAINVIEW_H
