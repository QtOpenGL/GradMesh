#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <subdiv/meshtools.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {

    Q_OBJECT

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
    void on_skeletonCB_toggled(bool checked);
    void on_ImportTRI_clicked();
    void on_makeNGonPB_clicked();
    void on_CCspinBox_valueChanged(int arg1);
    void on_levelSpinBox_valueChanged(int arg1);
    void on_colorPB_released();
    void on_alphaSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
