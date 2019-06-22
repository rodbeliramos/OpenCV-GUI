#include "mainwindow.h"
#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication openCV_GUI_App(argc, argv);
    MainWindow mainWin;
    mainWin.resize(800,600);
    mainWin.setMinimumSize(800,600);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    int height = screenGeometry.height();
    int width = screenGeometry.width();

    mainWin.move((width/2)-(400),(height/2)-300);
    mainWin.setWindowTitle("OpenCV Gui");
    mainWin.show();

    return openCV_GUI_App.exec();
}
