#ifndef COMPLEXSEARCHWINDOW_H
#define COMPLEXSEARCHWINDOW_H

#include <QDialog>
#include "mainWindow.h"

// Заголовочный файл окна создания фильтра поиска.

namespace Ui {
class ComplexSearchWindow;
}

class ComplexSearchWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ComplexSearchWindow(QWidget *parent = nullptr, SearchArgs* args = nullptr);
    ~ComplexSearchWindow();

private slots:


    void on_title_textChanged(const QString &arg1);

    void on_genre_textChanged(const QString &arg1);

    void on_date_textChanged(const QString &arg1);

    void on_min_rating_textChanged(const QString &arg1);

    void on_max_rating_textChanged(const QString &arg1);

    void on_startButton_clicked();

private:
    Ui::ComplexSearchWindow *ui;
    SearchArgs *searchArgs; // Изменяемый фильтр поиска.

    bool checkText(const string &title) const; // Типовой валидатор для текстового поля.
};

#endif // COMPLEXSEARCHWINDOW_H
