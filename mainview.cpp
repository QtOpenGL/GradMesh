#include "mainview.h"
#include "math.h"
#include "enums.h"

MainView::MainView(QWidget *parent)
    : QOpenGLWidget(parent)
    , rndrbles(new Renderables)
    , mouseHandler( new MouseHandler(rndrbles, this) )
{
    qDebug() << "✓✓ MainView constructor";

    mouseHandler->width = width();
    mouseHandler->height = height();
}

MainView::~MainView() {
    qDebug() << "✗✗ MainView destructor";

    delete mainShaderProg;
    delete blackShaderProg;
    delete whiteShaderProg;

    debugLogger->stopLogging();
    delete debugLogger;

    delete mouseHandler;
}

void MainView::createShaderPrograms() {
    mainShaderProg = new QOpenGLShaderProgram();
    mainShaderProg->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    mainShaderProg->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");
    mainShaderProg->link();

    blackShaderProg = new QOpenGLShaderProgram();
    blackShaderProg->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    blackShaderProg->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader_black.glsl");
    blackShaderProg->link();

    whiteShaderProg = new QOpenGLShaderProgram();
    whiteShaderProg->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    whiteShaderProg->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader_white.glsl");
    whiteShaderProg->link();

    greyShaderProg = new QOpenGLShaderProgram();
    greyShaderProg->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    greyShaderProg->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader_grey.glsl");
    greyShaderProg->link();
}

void MainView::createBuffer(){
    for (size_t i = 0; i < static_cast<size_t>(rndrbles->renderableList->size()); ++i)
        (*rndrbles->renderableList)[i]->registerRenderable(this);
}

void MainView::updateBuffers() {

}

void MainView::resizeGL(int newWidth, int newHeight) {

    Q_UNUSED(newWidth);
    Q_UNUSED(newHeight);

    qDebug() << ".. resizeGL";

    mouseHandler->width = width();
    mouseHandler->height = height();
}

void MainView::initializeGL() {

    initializeOpenGLFunctions();
    qDebug() << ":: OpenGL initialized";

    debugLogger = new QOpenGLDebugLogger();
    connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ), this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

//    if ( debugLogger->initialize() ) {
//      qDebug() << ":: Logging initialized";
//      debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
//      debugLogger->enableMessages();
//    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    glEnable(GL_PRIMITIVE_RESTART);
    maxInt = ((unsigned int) -1);
    glPrimitiveRestartIndex(maxInt);

    createShaderPrograms();
    createBuffer();
}

void MainView::renderRenderable(Renderable *obj, QOpenGLShaderProgram *shaderProg, GLenum mode){
    glBindVertexArray(obj->vao);
    if (mode == GL_POINTS){
        qDebug() << "Use renderRenderablePoints to render points";
        return;
    }
    bindShader(shaderProg);
    glDrawElements(mode, obj->indices->size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shaderProg->release();
    return;
}

void MainView::bindShader(QOpenGLShaderProgram *program){
    program->bind();
}

void MainView::renderRenderablePoints(Renderable *obj, size_t startIndex, size_t endIndex){
    glBindVertexArray(obj->vao);

    bindShader(blackShaderProg);
    glPointSize(8.0);
    glDrawArrays(GL_POINTS, startIndex, endIndex);

    bindShader(mainShaderProg);
    glPointSize(6.0);
    glDrawArrays(GL_POINTS, startIndex, endIndex);
    glBindVertexArray(0);
}


void MainView::paintGL() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Renderables *rndr = rndrbles;

    if (not rndr)
        return;

    if (rndr->colourSurface->hasMesh() && showColourSurface){
        (static_cast<Renderable *>(rndr->colourSurface))->updateRenderable(this);
        renderRenderable(static_cast<Renderable *>(rndr->colourSurface), mainShaderProg, GL_TRIANGLE_FAN);

    }

    if (rndr->colourSurface->hasMesh() && showSkeleton){
        (static_cast<Renderable *>(rndr->colourSurface))->updateRenderable(this);
        renderRenderable(static_cast<Renderable *>(rndr->colourSurface), blackShaderProg, GL_LINE_LOOP);
        renderRenderablePoints(static_cast<Renderable *>(rndr->colourSurface), 0, rndr->colourSurface->indices->size());
    }

    if (rndr->controlMesh->hasMesh() && showControlNet){
        (static_cast<Renderable *>(rndr->controlMesh))->updateRenderable(this);
        renderRenderable(static_cast<Renderable *>(rndr->controlMesh), blackShaderProg, GL_LINE_LOOP);
    }

    if (rndr->refineMesh->hasMesh()  && showRefine){
        (static_cast<Renderable *>(rndr->refineMesh))->updateRenderable(this);
        renderRenderable(static_cast<Renderable *>(rndr->refineMesh), mainShaderProg, GL_LINE_LOOP);
        int counter = -1;
        if (mouseHandler->selectedFace != -1){
            for (int i = 0; i <= mouseHandler->selectedFace; ++i){
                counter += 2 * rndr->controlMesh->mesh->Faces[i].val + 1;
            }
            renderRenderablePoints(static_cast<Renderable *>(rndr->refineMesh), counter, 1);
        }
    }

    if (rndr->skeletonMesh->hasMesh() && showGradients){
        (static_cast<Renderable *>(rndr->skeletonMesh))->updateRenderable(this);
        renderRenderablePoints(static_cast<Renderable *>(rndr->skeletonMesh), 0, rndr->skeletonMesh->indices->size());
    }

    int edgeIndex = mouseHandler->selectedEdge;
    if (edgeIndex != -1){
        (static_cast<Renderable *>(rndr->edgeRenderable))->updateRenderable(this);
        renderRenderable(rndr->edgeRenderable, whiteShaderProg, GL_LINES);
    }

    int faceIndex = mouseHandler->selectedFace;
    if (faceIndex != -1){
        (static_cast<Renderable *>(rndr->faceRenderable))->updateRenderable(this);
        renderRenderable(rndr->faceRenderable, whiteShaderProg, GL_LINE_LOOP);
    }

    update();
}

void MainView::mousePressEvent(QMouseEvent *event) {
    mouseHandler->mousePressEvent(event);
}
void MainView::mouseMoveEvent(QMouseEvent *event) {
  mouseHandler->mouseMoveEvent(event);
}
void MainView::mouseReleaseEvent(QMouseEvent *event){
    Q_UNUSED(event);
}
void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}
