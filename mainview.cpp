#include "mainview.h"
#include "math.h"
#include "enums.h"

MainView::MainView(QWidget *parent)
    : QOpenGLWidget(parent)
    , rndrbles(new QVector<Renderables *> (10 ,nullptr))
    , mouseHandler( new MouseHandler(rndrbles, this) )
{
    qDebug() << "✓✓ MainView constructor";
    for (size_t i = 0; i < 10; ++i)
        (*rndrbles)[i] = new Renderables;

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

    uniDx[0] = mainShaderProg->uniformLocation("dx");
    uniDy[0] = mainShaderProg->uniformLocation("dy");
    uniScale[0] = mainShaderProg->uniformLocation("scale");

    blackShaderProg = new QOpenGLShaderProgram();
    blackShaderProg->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    blackShaderProg->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader_black.glsl");
    blackShaderProg->link();

    uniDx[1] = blackShaderProg->uniformLocation("dx");
    uniDy[1] = blackShaderProg->uniformLocation("dy");
    uniScale[1] = blackShaderProg->uniformLocation("scale");

    whiteShaderProg = new QOpenGLShaderProgram();
    whiteShaderProg->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    whiteShaderProg->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader_white.glsl");
    whiteShaderProg->link();

    uniDx[2] = whiteShaderProg->uniformLocation("dx");
    uniDy[2] = whiteShaderProg->uniformLocation("dy");
    uniScale[2] = whiteShaderProg->uniformLocation("scale");

    greyShaderProg = new QOpenGLShaderProgram();
    greyShaderProg->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    greyShaderProg->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader_grey.glsl");
    greyShaderProg->link();

}

void MainView::createBuffer(size_t index){
    for (size_t i = 0; i < static_cast<size_t>((*rndrbles)[index]->renderableList->size()); ++i)
        ( (*((*rndrbles)[index])->renderableList)[i])->registerRenderable(this);
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
    for (size_t i = 0; i < 10; ++i)
        createBuffer(i);
}

void MainView::renderRenderable(Renderable *obj, QOpenGLShaderProgram *shaderProg, GLenum mode, size_t qdrnt){
    glBindVertexArray(obj->vao);
    if (mode == GL_POINTS){
        qDebug() << "Use renderRenderablePoints to render points";
        return;
    }
    bindShader(shaderProg, qdrnt);
    glDrawElements(mode, obj->indices->size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shaderProg->release();
    return;
}

void MainView::bindShader(QOpenGLShaderProgram *program, size_t qdrnt){
    program->bind();
    program->setUniformValue(uniDx[0], quadrantEntries[qdrnt][0]);
    program->setUniformValue(uniDy[0], quadrantEntries[qdrnt][1]);
    program->setUniformValue(uniScale[0], quadrantEntries[qdrnt][2]);
}

void MainView::renderRenderablePoints(Renderable *obj, size_t startIndex, size_t endIndex, size_t qdrnt){
    glBindVertexArray(obj->vao);

    bindShader(blackShaderProg, qdrnt);
    glPointSize(8.0);
    glDrawArrays(GL_POINTS, startIndex, endIndex);

    bindShader(mainShaderProg, qdrnt);
    glPointSize(6.0);
    glDrawArrays(GL_POINTS, startIndex, endIndex);
    glBindVertexArray(0);
}


void MainView::paintGL() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Renderables *rndr =
            disp == DISPLAY::ORIGINAL ? (*rndrbles)[0] :
            disp == DISPLAY::MESHREF ? (*rndrbles)[1] :
            disp == DISPLAY::CTRLVECTOR ? (*rndrbles)[2] : nullptr;

    if (not rndr){
        return;
    }

    if (rndr->colourSurface->hasMesh() && showColourSurface){
        (static_cast<Renderable *>(rndr->colourSurface))->updateRenderable(this);
        renderRenderable(static_cast<Renderable *>(rndr->colourSurface), mainShaderProg, GL_TRIANGLE_FAN, rndr->quadrant);
        renderRenderable(static_cast<Renderable *>(rndr->colourSurface), greyShaderProg, GL_LINE_LOOP, rndr->quadrant);
    }

    if (rndr->colourSurface->hasMesh() && showSkeleton){
        (static_cast<Renderable *>(rndr->colourSurface))->updateRenderable(this);
        renderRenderable(static_cast<Renderable *>(rndr->colourSurface), blackShaderProg, GL_LINE_LOOP, rndr->quadrant);
        renderRenderablePoints(static_cast<Renderable *>(rndr->colourSurface), 0, rndr->colourSurface->indices->size(), rndr->quadrant);
    }

    if (rndr->controlMesh->hasMesh() && showControlNet){
        (static_cast<Renderable *>(rndr->controlMesh))->updateRenderable(this);
        renderRenderable(static_cast<Renderable *>(rndr->controlMesh), blackShaderProg, GL_LINE_LOOP, rndr->quadrant);
    }

    if (rndr->skeletonMesh->hasMesh() && showGradients){
        (static_cast<Renderable *>(rndr->skeletonMesh))->updateRenderable(this);
        renderRenderablePoints(static_cast<Renderable *>(rndr->skeletonMesh), 0, rndr->skeletonMesh->indices->size(), rndr->quadrant);
    }

    int edgeIndex = mouseHandler->selectedEdge;
    if (edgeIndex != -1){
        (static_cast<Renderable *>(rndr->edgeRenderable))->updateRenderable(this);
        renderRenderable(rndr->edgeRenderable, whiteShaderProg, GL_LINES, rndr->quadrant);
    }

    int faceIndex = mouseHandler->selectedFace;
    if (faceIndex != -1){
        (static_cast<Renderable *>(rndr->faceRenderable))->updateRenderable(this);
        renderRenderable(rndr->faceRenderable, whiteShaderProg, GL_LINE_LOOP, rndr->quadrant);
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
