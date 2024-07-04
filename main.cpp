#include "mainwindow.h"
#include <QApplication>
#include "windows.h"
#include "startwindow.h"
#include <QTimer>

int main(int argc, char *argv[])
{
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    QApplication a(argc, argv);

    StartWindow *start = new StartWindow();
    start->show();
    QTimer::singleShot(3000, [start]() {
        start->close();
        delete start;

        MainWindow *w = new MainWindow();
        w->show();
    });

    return a.exec();
}
