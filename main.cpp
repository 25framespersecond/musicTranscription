#include "mainwindow.h"
#include <QApplication>
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include "wavfile.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();
}
