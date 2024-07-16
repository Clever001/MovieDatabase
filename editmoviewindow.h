#ifndef EDITMOVIEWINDOW_H
#define EDITMOVIEWINDOW_H

#include <QDialog>
#include <QListWidget>
#include <list>
#include "MovieModel.h"

// Окно редактирования фильма.

namespace Ui {
class EditMovieWindow;
}

class EditMovieWindow : public QDialog
{
    Q_OBJECT

public:
    explicit EditMovieWindow(QWidget *parent = nullptr, Movie *curMovie = nullptr, MoviesManager * manager = nullptr);
    ~EditMovieWindow();

private slots:
    void on_editTitle_textChanged(const QString &arg1);

    void on_editDate_textChanged(const QString &arg1);

    void on_editGenre_textChanged(const QString &arg1);

    void on_addGenreButton_clicked();

    void on_editGenresList_itemDoubleClicked(QListWidgetItem *item);


    void on_editRating_textChanged(const QString &arg1);

    void on_loadPosterButton_clicked();

    void on_editMovieButton_clicked();

private:
    Ui::EditMovieWindow *ui;
    Movie *curMovie; // Указатель на копию редактируемого фильма.
    MoviesManager *manager; // Указатель на контроллер.
    string oldPosterName, newPosterName;
    string originalName, originalDate;
    list<string> genres;
    bool isEdited; // Нужно чтобы понять, удалять новый постер или нет.


    bool checkText(const string &title) const; // Валидация.
    bool checkRating(const string &rating) const; // Валидация.
    void deleteOldPoster();
    void deleteNewPoster();
};

#endif // EDITMOVIEWINDOW_H
