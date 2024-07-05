#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newmoviewindow.h"
#include "editmoviewindow.h"
#include "complexsearchwindow.h"
#include <vector>
#include <QMessageBox>
#include <QFileDialog>
#include <QDate>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dataPath = QDir::currentPath() + "/data/";
    moviesOutput.push_back(MovieOutput(ui->movie1Label, ui->movie1Date, ui->movie1Poster));
    moviesOutput.push_back(MovieOutput(ui->movie2Label, ui->movie2Date, ui->movie2Poster));
    moviesOutput.push_back(MovieOutput(ui->movie3Label, ui->movie3Date, ui->movie3Poster));
    moviesOutput.push_back(MovieOutput(ui->movie4Label, ui->movie4Date, ui->movie4Poster));
    manager = new MoviesManager();
    movies = manager->search();
    changeOffset(0);
    printMovies();
    ui->statusBar->showMessage("Приложение запущено успешно!");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete manager;
}

void MainWindow::changeOffset(size_t val){
    offset = val;
    ui->offsetLabel->setText("Отступ: " + QString::fromStdString(to_string(val)));
}

void MainWindow::printMovies() const {
    size_t cnt = movies.size();
    for (size_t i = 0; i != 4; ++i) {
        if (i + offset < cnt){
            moviesOutput[i].label->setText(QString::fromStdString(movies[i + offset]->title));
            moviesOutput[i].date->setText(QString::fromStdString(movies[i + offset]->release_date));
            QString curPos = dataPath + "posters/" + QString::fromStdString(movies[i + offset]->poster);
            if (QFile::exists(curPos)){
                QPixmap pixmap(curPos);
                int h = moviesOutput[i].poster->height();
                int w = moviesOutput[i].poster->width();
                moviesOutput[i].poster->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
            } else {
                moviesOutput[i].poster->clear();
                moviesOutput[i].poster->setText("Нет фильма");
            }
        }
        else {
            moviesOutput[i].label->setText(QString::fromStdString("Нет фильма"));
            moviesOutput[i].date->setText(QString::fromStdString("Нет фильма"));
            moviesOutput[i].poster->clear();
            moviesOutput[i].poster->setText("Нет фильма");
        }
    }
}

void MainWindow::on_newFilmAction_triggered()
{
    size_t old_size = manager->size();
    NewMovieWindow window(this, manager);
    window.exec();
    if (old_size + 1 == manager->size()) {
        movies = manager->search();
        printMovies();
        ui->statusBar->showMessage("Был добавлен новый фильм.");
    }
    else if (old_size == manager->size())
        ui->statusBar->showMessage("Новый фильм не был добавлен.");
    else {
        QMessageBox::warning(this, "Ошибка","Ошибка в работе приложения.");
        close();
    }
}

void MainWindow::editMovie(const Movie *curMovie) {
    if (curMovie == nullptr){
        QMessageBox::warning(this, "Изменение фильма", "Такого фильма нет.");
        return;
    }
    Movie tmp = *curMovie;
    EditMovieWindow window(this, &tmp, manager);
    window.exec();
    if (tmp == *curMovie){
        ui->statusBar->showMessage("Фильм не был изменен.");
    }
    else if (manager->editMovie(curMovie->title, curMovie->release_date, tmp)){
        movies = manager->search();
        changeOffset(0);
        printMovies();
        ui->statusBar->showMessage("Фильм был успешно изменен.");
    }
    else {
        QMessageBox::warning(this, "Ошибка", "Ошибка в работе приложения.");
        close();
    }

}

void MainWindow::on_movie1EditButton_clicked()
{
    if (offset < movies.size())
        editMovie(movies[offset]);
    else
        editMovie(nullptr);
}

void MainWindow::on_movie2EditButton_clicked()
{
    if (offset + 1 < movies.size())
        editMovie(movies[offset + 1]);
    else
        editMovie(nullptr);
}


void MainWindow::on_movie3EditButton_clicked()
{
    if (offset + 2 < movies.size())
        editMovie(movies[offset + 2]);
    else
        editMovie(nullptr);
}

void MainWindow::on_movie4EditButton_clicked()
{
    if (offset + 3 < movies.size())
        editMovie(movies[offset + 3]);
    else
        editMovie(nullptr);
}

void MainWindow::on_complexSearchButton_clicked()
{
    SearchArgs searchArgs;
    ComplexSearchWindow window(this, &searchArgs);
    window.exec();

    movies = manager->search(searchArgs.title, searchArgs.genre, searchArgs.release_date, searchArgs.min_rating, searchArgs.max_rating);
    changeOffset(0);
    printMovies();
    ui->statusBar->showMessage("Фильтр поиска успешно изменен!");
}

void MainWindow::on_saveAction_triggered()
{
    QString savePath = dataPath + "json/";
    QDir().mkpath(savePath);
    savePath += "data.json";
    try {
        manager->saveToDb(savePath.toStdString());
        QMessageBox::information(this, "Сохранение информации", "Информация успешно сохранена.");
        ui->statusBar->showMessage("Информация успешно сохранена.");
    } catch (...) {
        QMessageBox::warning(this, "Сохранение информации", "Возникла ошибка при сохранении информации.");
        ui->statusBar->showMessage("Информация не была сохранена.");
    }
}

void MainWindow::on_loadAction_triggered()
{
    const QString savePath = dataPath + "json/data.json";
    if (QFile::exists(savePath)){
        try {
            manager->loadFromDb(savePath.toStdString());
            movies = manager->search();
            changeOffset(0);
            printMovies();
            QMessageBox::information(this, "Получение информации", "Информация успешно загружена с диска.");
            ui->statusBar->showMessage("Информация загружена с диска.");
        } catch (...) {
            QMessageBox::warning(this, "Получение информации", "Возникла ошибка при получении информации.");
            ui->statusBar->showMessage("Возникла ошибка при загрузке информации с диска.");
        }
    } else {
        QMessageBox::information(this, "Получение информации", "Возникла ошибка при получении информации. Убедитесь, была ли сохранена информация ранее.");
        ui->statusBar->showMessage("Возникла ошибка при загрузке информации с диска.");
    }
}

void MainWindow::on_nextButton_clicked()
{
    if (offset < movies.size())
        changeOffset(offset + 1);
    printMovies();
}

void MainWindow::on_previousButton_clicked()
{
    if (offset > 0)
        changeOffset(offset - 1);
    printMovies();
}

void MainWindow::deleteMovie(size_t index) {
    if (offset + index < movies.size()){
        QString posterPath = dataPath + "posters/" + QString::fromStdString(movies[offset + index]->poster);
        if (QFile::exists(posterPath))
            QFile::remove(posterPath);
        manager->removeMovie(movies[offset + index]->title, movies[offset + index]->release_date);
        movies = manager->search();
        changeOffset(0);
        printMovies();
        QMessageBox::information(this, "Удаление фильма", "Фильм успешно удален.");
        ui->statusBar->showMessage("Фильм успешно удален.");
    } else {
        QMessageBox::warning(this, "Удаление фильма", "Такого фильма нет.");
        ui->statusBar->showMessage("Такого фильма нет.");
    }
}

void MainWindow::on_movie1DelButton_clicked()
{
    deleteMovie(0);
}

void MainWindow::on_movie2DelButton_clicked()
{
    deleteMovie(1);
}

void MainWindow::on_movie3DelButton_clicked()
{
    deleteMovie(2);
}

void MainWindow::on_movie4DelButton_clicked()
{
    deleteMovie(3);
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    movies = manager->search(ui->lineEdit->text().toStdString());
    changeOffset(0);
    printMovies();
    if (arg1.isEmpty()){
        ui->statusBar->showMessage("Вывод всех фильмов.");
    } else {
        ui->statusBar->showMessage("Вывод фильмов с названиме, похожим на \"" + arg1 + "\".");
    }
}

void MainWindow::on_actionnotReleasedAction_triggered()
{
    movies = manager->searchNotReleased(QDate::currentDate().toString("dd.MM.yyyy").toStdString());
    changeOffset(0);
    printMovies();
    ui->statusBar->showMessage("Показаны еще не выпущенные в прокат фильмы.");
}

void MainWindow::on_viewAllFilmsAction_triggered()
{
    movies = manager->search();
    changeOffset(0);
    printMovies();
    ui->statusBar->showMessage("Вывод всех фильмов.");
}

void MainWindow::on_aboutAction_triggered()
{
    QMessageBox::about(this, "О программе", "Разработчик программы: студент группы КЭ-243 Никончук Виталий. "
                                                  "\n\nПрограмма предназначена для работы с информацией о фильмах. Проект предоставляет возможность сохранения, "
                                                  "изменения и просмотра информации о фильмах. Информацию можно сохранять в файле формата json. "
                                                  "\n\nЮУрГУ Июль 2024.");
    QMessageBox::aboutQt(this, "О платформе");
}

void MainWindow::on_helpAction_triggered()
{
    QMessageBox::information(this, "Помощь", "<h2>Создание фильма</h2><p>Для создания фильма выберите пункт \"Фильмы->Создать новый фильм\" в основном окне. Далее нужно заполнить поля формы. "
                             "Каждое поле подсвечивается красным, если оно было заполнено не валидными данными. Если вы что-то сделали не так при заполнении информации о фильме"
                             ", то программа об этом сообщит. Для удаления некоректных жанров просто нажмите на него два раза.</p>"
                             "<h2>Просмотр/изменение информации о фильме</h2><p>В основном окне нажмите кнопку \"info\" под требуемым фильмом. Измените информацию так, как вы пожелаете. "
                             "Для сохранения изменений нажмите изменить фильм, иначе просто закройте диалоговое окно.</p>"
                             "<h2>Удаление фильма</h2><p>В основном окне нажмите кнопку \"Удалить\" под удаляемым фильмом.</p>"
                             "<h2>Установка фильтра поиска</h2><p>В основном окне нажмите \"Использовать сложный поиск\". Заполните необходимые поля (все поля необязательные к заполнению). "
                             "Примените фильтр.</p>"
                             "<h2>Поиск не вышедших фильмов</h2><p>Для поиска невышедших фильмов выберите пункт \"Фильмы->Поиск невышедших фильмов\" в основном окне программы.</p>"
                             "<h2>Сохранение/загрузка изменений</h2><p>Нажимайте соответствующие кнопки меню в пункте \"Файл\".</p>");
}
