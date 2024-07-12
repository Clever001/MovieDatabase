#ifndef NEWMOVIEWINDOW_H
#define NEWMOVIEWINDOW_H

#include <QDialog>
#include <QListWidget>
#include "MovieModel.h"
#include <list>

namespace Ui {
class NewMovieWindow;
}

class NewMovieWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NewMovieWindow(QWidget *parent = nullptr, MoviesManager* m = nullptr);
    ~NewMovieWindow();

private slots:
    void on_newTitle_textChanged(const QString &arg1);

    void on_newDate_textChanged(const QString &arg1);

    void on_newGenre_textChanged(const QString &arg1);

    void on_addGenreButton_clicked();

    void on_newRating_textChanged(const QString &arg1);

    void on_loadPosterButton_clicked();

    void on_addMovieButton_clicked();

    void on_newGenresList_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::NewMovieWindow *ui;
    MoviesManager *manager;
    list<string> genres;
    string posterName;
    bool movieIsSaved;

    bool checkText(const string& title) const;
    bool checkRating(const string& rating) const;
    void deleteCurPoster();
};

#endif // NEWMOVIEWINDOW_H
