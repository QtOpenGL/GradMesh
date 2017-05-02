#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <subdiv/meshtools.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {

    Q_OBJECT

    size_t currentMesh = 0;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_ImportOBJ_clicked();
    void on_clickCB_currentIndexChanged(const QString &arg1);
    void on_gradientCB_toggled(bool checked);
    void on_controlNetCB_toggled(bool checked);
    void on_colourSurfaceCB_toggled(bool checked);
    void on_gradSlider_valueChanged(int value);
    void on_refineFacePB_clicked();
    void on_skeletonCB_toggled(bool checked);

    void on_methodLE_editingFinished();

    void on_ImportTRI_clicked();


    void on_spinBox_valueChanged(int arg1);

    void on_makeNGonPB_clicked();

    void on_toStringPB_clicked();

    void on_toNewMeshPB_clicked();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
