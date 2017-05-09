#include "renderable.h"
#include "mainview.h"

Renderable::Renderable()
    :
      coords( new QVector<QVector2D> ),
      colours( new QVector<QVector3D> ),
      indices( new QVector<unsigned int> )
{

}


void Renderable::registerRenderable(MainView *ui){

    ui->glGenVertexArrays(1, &vao);
    ui->glBindVertexArray(vao);

    ui->glGenBuffers(1, &coordsBO);
    ui->glBindBuffer(GL_ARRAY_BUFFER, coordsBO);
    ui->glEnableVertexAttribArray(0);
    ui->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    ui->glGenBuffers(1, &colourBO);
    ui->glBindBuffer(GL_ARRAY_BUFFER, colourBO);
    ui->glEnableVertexAttribArray(1);
    ui->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    ui->glGenBuffers(1, &indicesBO);
    ui->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBO);

    ui->glBindVertexArray(0);
    isRegistered = true;
}


void Renderable::updateRenderable(MainView *ui){
    ui->glBindBuffer(GL_ARRAY_BUFFER, coordsBO);
    ui->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D) * coords->size(), coords->data(), GL_DYNAMIC_DRAW);

    ui->glBindBuffer(GL_ARRAY_BUFFER, colourBO);
    ui->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * colours->size(), colours->data(), GL_DYNAMIC_DRAW);

    ui->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBO);
    ui->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices->size(), indices->data(), GL_DYNAMIC_DRAW);
}


void Renderable::fillNonsense(){
    coords->clear();
    coords->squeeze();
    colours->clear();
    colours->squeeze();
    indices->clear();
    colours->squeeze();

    *coords = QVector <QVector2D> ({
                                       {-0.5, -0.5},
                                       {-0.5, 0.5},
                                       {0.5, 0.5},
                                       {0.5, -0.5}
                                   });

    *colours = QVector <QVector3D> ({
                                        {0.5, 0.5, 0.5},
                                        {0.5, 0.5, 0.5},
                                        {0.5, 0.5, 0.5},
                                        {0.5, 0.5, 0.5}
                                    });

    *indices = QVector <unsigned int> ({0, 1,2,3,((unsigned int) - 1)});
}

Renderable::~Renderable(){
    delete coords;
    delete colours;
    delete indices;
}


