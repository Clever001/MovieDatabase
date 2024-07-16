#include "mainwindow.h"
#include <QApplication>
#include "windows.h"
#include "startwindow.h"
#include <QTimer>

int main(int argc, char *argv[])
{
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001); // Устанавливается кодировка консоли UTF-8 на всякий случай.
    QApplication a(argc, argv);

    StartWindow *start = new StartWindow(); // Показывается заставка
    start->show();
    QTimer::singleShot(3000, [start]() {
        // Через 3 секунды заставка закрывается. Показывается главное окно.
        start->close();
        delete start;

        MainWindow *w = new MainWindow();
        w->setAttribute(Qt::WA_DeleteOnClose); // Данный флаг нужен для очистки памяти после closeEvent.
        w->show();
    });

    return a.exec();
}
