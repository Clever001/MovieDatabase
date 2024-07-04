#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "MovieModel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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
    Ui::MainWindow *ui;
    MoviesManager *manager;
    vector<const Movie*> movies;
    size_t offset; // window index


    void editMovie(const Movie *curMovie);
    void printMovies() const;
    void deleteMovie(size_t index);
};

struct MovieOutput{
    QLabel *label, *date;
    MovieOutput(QLabel* label, QLabel* date){
        this->label = label;
        this->date = date;
    }
};

struct SearchArgs{
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
