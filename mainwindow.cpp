#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "enums.h"
#include "subdiv/subdiv.h"
#include "subdiv/subdiv.h"
#include <assert.h>

MainWindow::MainWindow(QWidget *parent) :  QMainWindow(parent), ui(new Ui::MainWindow) {
  qDebug() << "✓✓ MainWindow constructor";
  ui->setupUi(this);
}

MainWindow::~MainWindow() {
  qDebug() << "✗✗ MainWindow destructor";
  delete ui;
}

void MainWindow::on_ImportOBJ_clicked() {
    OBJFile newModel = OBJFile(QFileDialog::getOpenFileName(this, "Import OBJ File", "../../CAD2/CAD/models/", tr("Obj Files (*.obj)")));

    delete ui->mainView->rndrbles->controlMesh->mesh;
    ui->mainView->rndrbles->init(&newModel);
    ui->mainView->updateBuffers();
    ui->mainView->mouseHandler->isStarted = true;
}

void MainWindow::on_gradientCB_toggled(bool checked)
{
    ui->mainView->showGradients = checked;
}

void MainWindow::on_controlNetCB_toggled(bool checked)
{
    ui->mainView->showControlNet = checked;
    if (checked)
        ui->mainView->showSkeleton = false;
}

void MainWindow::on_colourSurfaceCB_toggled(bool checked)
{
    ui->mainView->showColourSurface = checked;
}

void MainWindow::on_skeletonCB_toggled(bool checked)
{
    ui->mainView->showSkeleton = checked;
    if (checked)
        ui->mainView->showControlNet = false;
}


void MainWindow::on_clickCB_currentIndexChanged(const QString &arg1)
{ // Change the type of shading in use
    ui->mainView->mouseHandler->selectType =
            arg1 == "Points"     ? POINTS :
            arg1 == "Edges"      ? EDGES  :
                                   FACES  ;

    ui->mainView->mouseHandler->selectedEdge = -1;
    ui->mainView->mouseHandler->selectedFace = -1;
    ui->mainView->mouseHandler->selectedPt = -1;
}

void MainWindow::on_gradSlider_valueChanged(int value)
{
    if (not ui->mainView->mouseHandler->isStarted)
        return;

    float ratio = (float) value / 100.0f;

    QVector<HalfEdge> *HalfEdges = &(ui->mainView->rndrbles->controlMesh->mesh->HalfEdges);
    HalfEdge *currentEdge;
    for (int i = 0; i < HalfEdges->size(); ++i){
        currentEdge = &(*HalfEdges)[i];
        currentEdge->colGrad = ratio * (currentEdge->target->coords - currentEdge->twin->target->coords);
    }
    ui->mainView->rndrbles->updateEm();
}

void MainWindow::on_refineFacePB_clicked()
{
    int selectedFace = ui->mainView->mouseHandler->selectedFace;
    if (selectedFace == -1){
        qDebug() << "No face selected";
        return;
    }
    OBJFile obj =  ccSingleFace(
                ui->mainView->rndrbles->controlMesh->mesh,
                ui->mainView->mouseHandler->selectedFace
                );

//    qDebug() << obj.vertexCoords;
    ui->mainView->rndrbles->controlMesh->mesh = new Mesh;
    ui->mainView->rndrbles->init(&obj);
}

void MainWindow::on_methodLE_editingFinished()
{
    if (not ui->mainView->mouseHandler->isStarted)
        return;

    QString text = ui->methodLE->text();
    Renderables *rndrable = ui->mainView->rndrbles;

    rndrable->refiners.clear();
    rndrable->refiners.squeeze();

    for (unsigned int letter = 0; letter < static_cast<unsigned int>(text.size()); ++letter){
        if (text[letter] == 'T' || text[letter] == 't')
            rndrable->refiners.append({&ternaryStep, TERNARY});
        else if (text[letter] == 'C' || text[letter] == 'c')
            rndrable->refiners.append({&subdivideCatmullClark, CATMULLCLARK});
        else
            qDebug() << "Invalid input";
    }
    rndrable->updateEm();
}

void MainWindow::on_ImportTRI_clicked()
{
    OBJFile newModel = OBJFile("../../CAD2/CAD/models/tri1.obj");

    delete ui->mainView->rndrbles->controlMesh->mesh;
    ui->mainView->rndrbles->init(&newModel);

    ui->mainView->updateBuffers();
    ui->mainView->mouseHandler->isStarted = true;
}

void MainWindow::on_makeNGonPB_clicked()
{
    OBJFile obj = makeNGON(5);
    ui->mainView->rndrbles->controlMesh->mesh = new Mesh;
    ui->mainView->rndrbles->init(&obj);
}

void MainWindow::on_toStringPB_clicked()
{
    Mesh *mesh = ui->mainView->rndrbles->colourSurface->mesh;
    QString str;
    Vertex *vtx;
    Face *face;
    for (int i = 0; i < mesh->Vertices.size(); ++i){
        vtx = &mesh->Vertices[i];
        str.append(vtxToString(vtx));
        str.append("\n");
    }
    str.append("s off\n");
    for (int i = 0; i < mesh->Faces.size(); ++i){
        face = &mesh->Faces[i];
        str.append(faceToString(face));
        str.append("\n");
    }

    qDebug().noquote() << str;
}

void MainWindow::on_toNewMeshPB_clicked()
{
    Mesh *mesh = ui->mainView->rndrbles->colourSurface->mesh;
    QString str;
    Vertex *vtx;
    Face *face;
    for (int i = 0; i < mesh->Vertices.size(); ++i){
        vtx = &mesh->Vertices[i];
        str.append(vtxToString(vtx));
        str.append("\n");
    }
    str.append("s off\n");
    for (int i = 0; i < mesh->Faces.size(); ++i){
        face = &mesh->Faces[i];
        str.append(faceToString(face));
        str.append("\n");
    }

    delete mesh;
    mesh = new Mesh;
    QFile caFile("output.txt");
    caFile.open(QIODevice::WriteOnly | QIODevice::Text);

    assert(caFile.isOpen());
    QTextStream outStream(&caFile);
    outStream << str;
    caFile.close();

    OBJFile obj("output.txt");


    ui->mainView->rndrbles->init(&obj);
}

void MainWindow::on_refineCB_toggled(bool checked)
{
    ui->mainView->showRefine = checked;
}
