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
    bindShader(shaderProg);
    glDrawElements(mode, obj->indices->size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shaderProg->release();
    return;
}

void MainView::bindShader(QOpenGLShaderProgram *program){
    program->bind();
}

void MainView::renderRenderablePoints(Renderable *obj, size_t startIndex, size_t count, bool coloured){
    glBindVertexArray(obj->vao);

    bindShader(blackShaderProg);
    glPointSize(8.0);
    glDrawArrays(GL_POINTS, startIndex, count);

    if (coloured)
        bindShader(mainShaderProg);
    else
        bindShader(whiteShaderProg);

    glPointSize(6.0);
    glDrawArrays(GL_POINTS, startIndex, count);
    glBindVertexArray(0);
}

void MainView::paintGL() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Renderables *rndr = rndrbles;

    if (not rndr)
        return;

    for (size_t i = 0; i < static_cast<size_t>(rndr->meshVector.size()); ++i)
        if (not rndr->meshVector[i]->isRegistered)
            rndr->meshVector[i]->registerRenderable(this);

    if (ref_level != 0){
        rndr->meshVector[ref_level]->fillCoords();
        rndr->meshVector[ref_level]->fillColours();
    }

    if (rndr->colourSurface->hasMesh() && showColourSurface){
        (static_cast<Renderable *>(rndr->colourSurface))->updateRenderable(this);
        renderRenderable(static_cast<Renderable *>(rndr->colourSurface), mainShaderProg, GL_TRIANGLE_FAN);
    }

    if (rndr->colourSurface->hasMesh() && showSkeleton){
        (static_cast<Renderable *>(rndr->colourSurface))->updateRenderable(this);
        renderRenderablePoints(static_cast<Renderable *>(rndr->colourSurface), 0, rndr->colourSurface->indices->size(), true);
    }

    if (rndr->controlMesh->hasMesh() && showControlNet){
        if (ref_level == 0){
            (static_cast<Renderable *>(rndr->controlMesh))->updateRenderable(this);
            renderRenderable(static_cast<Renderable *>(rndr->controlMesh), greyShaderProg, GL_LINE_LOOP);
        }
        if (ref_level > 0){
            rndr->meshVector[ref_level]->indices->clear();
            rndr->meshVector[ref_level]->indices->squeeze();
            for (size_t index : *rndr->ptIndices[ref_level - 1])
                rndr->meshVector[ref_level]->indices->append(index);

            (static_cast<Renderable *>(rndr->meshVector[ref_level]))->updateRenderable(this);
            renderRenderable(static_cast<Renderable *>(rndr->meshVector[ref_level]), greyShaderProg, GL_LINE_LOOP);
            if (mouseHandler->selectedPt != -1)
                renderRenderablePoints(static_cast<Renderable *>(rndr->meshVector[ref_level]), mouseHandler->selectedPt, 1, true);
        }
    }

    if (rndr->skeletonMesh->hasMesh() && showGradients){
        if (ref_level == 0){
            (static_cast<Renderable *>(rndr->skeletonMesh))->updateRenderable(this);
            renderRenderablePoints(static_cast<Renderable *>(rndr->skeletonMesh), 0, rndr->skeletonMesh->indices->size(), true);
        }
        else {
            rndr->gradRenderable->coords->clear();
            rndr->gradRenderable->coords->squeeze();
            rndr->gradRenderable->colours->clear();
            rndr->gradRenderable->colours->squeeze();
            QVector<Vertex> *vertices = &rndr->meshVector[ref_level]->mesh->Vertices;
            for (auto &ctr : (*rndr->controlVectors)[ref_level - 1]){
                for (auto &vec : *(std::get<3>(ctr)))
                    rndr->gradRenderable->coords->append(vec + (*vertices)[std::get<0>(ctr)].coords);
            }
            rndr->gradRenderable->updateRenderable(this);
            renderRenderablePoints(rndr->gradRenderable, 0, rndr->gradRenderable->coords->size(), false);
        }
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
