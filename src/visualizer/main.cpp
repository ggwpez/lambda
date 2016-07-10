#include "mainwindow.hpp"
#include <QApplication>
#include <QWidget>
#include <interpreter.hpp>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
