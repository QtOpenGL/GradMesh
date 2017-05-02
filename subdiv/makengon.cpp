#include "subdiv.h"
#include "QFile"
#include "QTextStream"
#include <assert.h>

OBJFile makeNGON(size_t n){

    QString str;
    for (size_t i = 0 ; i < n; ++i){
        str.append("v " + QString::number(cos(2 * i * M_PI / ((float) n))) + " " + QString::number(sin(2 * i * M_PI / ((float) n))) + " ");
        str.append("0.0 0.0 0.0\n");
    }

    str.append("f ");
    for (size_t i = 0; i < n - 1; ++i)
        str.append(QString::number(i + 1) + " ");
    str.append(QString::number(n) + "\n");


    qDebug() << str;
    QFile caFile("output.txt");
    caFile.open(QIODevice::WriteOnly | QIODevice::Text);

    assert(caFile.isOpen());
    QTextStream outStream(&caFile);
    outStream << str;
    caFile.close();

    OBJFile obj("output.txt");
    return obj;
}
