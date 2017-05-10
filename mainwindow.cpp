#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "enums.h"
#include "subdiv/subdiv.h"
#include "subdiv/subdiv.h"
#include <QColorDialog>
#include <assert.h>
#include <dialog.h>

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
}

void MainWindow::on_colourSurfaceCB_toggled(bool checked)
{
    ui->mainView->showColourSurface = checked;
}

void MainWindow::on_skeletonCB_toggled(bool checked)
{
    ui->mainView->showSkeleton = checked;
}


void MainWindow::on_clickCB_currentIndexChanged(const QString &arg1)
{ // Change the type of shading in use
    ui->mainView->mouseHandler->selectType =
            arg1 == "Points"     ? POINTS :
            arg1 == "Gradients"  ? GRADS  :
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
    delete ui->mainView->rndrbles->controlMesh->mesh;
    ui->mainView->rndrbles->init(&obj);
    ui->mainView->updateBuffers();
}

void MainWindow::on_CCspinBox_valueChanged(int arg1)
{
    ui->mainView->rndrbles->ccSteps = arg1;
    ui->mainView->rndrbles->updateEm();
}

void MainWindow::on_levelSpinBox_valueChanged(int arg1)
{
    ui->mainView->ref_level = arg1;
}

void MainWindow::on_colorPB_released()
{
    Dialog dia;
    dia.setModal(true);
    dia.exec();
}

void MainWindow::on_alphaSlider_valueChanged(int value)
{
    unsigned int selectedPt = ui->mainView->mouseHandler->selectedPt;
    if (selectedPt == (unsigned long)(-1))
        return;

    int ref_level = ui->mainView->ref_level;
    if (ref_level == 0)
        return;

    for (auto &t : (*ui->mainView->rndrbles->controlVectors)[ref_level - 1]){
        if (std::get<0>(t) == selectedPt)
            std::get<2>(t) = value / 40.0f;
    }

    ui->mainView->rndrbles->updateEm();
}
