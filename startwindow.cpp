#include "startwindow.h"
#include "ui_startwindow.h"
#include <QPixmap>

StartWindow::StartWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartWindow)
{
    ui->setupUi(this);

    // Установка картинки заставки из файла ресурсов.
    QPixmap pix(":/projectFiles/data/projectImages/screensaver.jpg");

    int h = ui->screensaver->height();
    int w = ui->screensaver->width();

    ui->screensaver->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
}

StartWindow::~StartWindow()
{
    delete ui;
}
