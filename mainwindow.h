#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "MovieModel.h"

namespace Ui {
class MainWindow; // класс главного окна.
}

struct MovieOutput;
struct SearchArgs;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_newFilmAction_triggered();

    void on_movie1EditButton_clicked();

    void on_movie2EditButton_clicked();

    void on_movie3EditButton_clicked();

    void on_movie4EditButton_clicked();

    void on_complexSearchButton_clicked();

    void on_saveAction_triggered();

    void on_loadAction_triggered();

    void on_nextButton_clicked();

    void on_previousButton_clicked();

    void on_movie1DelButton_clicked();

    void on_movie2DelButton_clicked();

    void on_movie3DelButton_clicked();

    void on_movie4DelButton_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_actionnotReleasedAction_triggered();

    void on_viewAllFilmsAction_triggered();

    void on_aboutAction_triggered();

    void on_helpAction_triggered();

private:
    Ui::MainWindow *ui; // Управление виджетами на окне.
    MoviesManager *manager; // управление контроллером.
    vector<const Movie*> movies; // Нужен для реализации области предпросмотра.
    size_t offset; // Отступ области предпросмотра от начала.
    vector<MovieOutput> moviesOutput; // Нужен для более удобного вывода фильмов в область предпросмотра.
    QString dataPath; // Путь к папке data.


    void editMovie(const Movie *curMovie); // Запуск окна редактирования фильма.
    void printMovies() const; // Вывод фильмов в область предпросмотра.
    void deleteMovie(size_t index); // Удаление фильма.
    void changeOffset(size_t val); //  Изменяет отступ и выводит информацию об этом в status bar.
};

struct MovieOutput{
    // Структура нужна для более удобного вывода фильмов в область предпросмотра.
    QLabel *label, *date, *poster;
    MovieOutput(QLabel* label, QLabel* date, QLabel* poster):
        label(label), date(date), poster(poster) {}
};

struct SearchArgs{
    // Нужна для реализации фильтра поиска. При открытии окна поиска не создается новый фильм, а изменяется переменная данного типа.
    string title;
    string genre;
    string release_date;
    double min_rating;
    double max_rating;
    SearchArgs(const string& title = "", const string& genre = "", const string& release_date = "",
               double min_rating = -1., double max_rating = -1.):
        title(title), genre(genre), release_date(release_date), min_rating(min_rating),
        max_rating(max_rating){}
};

#endif // MAINWINDOW_H
