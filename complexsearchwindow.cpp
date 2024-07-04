#include "complexsearchwindow.h"
#include "ui_complexsearchwindow.h"
#include <QMessageBox>

ComplexSearchWindow::ComplexSearchWindow(QWidget *parent, SearchArgs* args) :
    QDialog(parent),
    ui(new Ui::ComplexSearchWindow),
    searchArgs(args)
{
    ui->setupUi(this);
}

ComplexSearchWindow::~ComplexSearchWindow()
{
    delete ui;
}
bool ComplexSearchWindow::checkText(const string &title) const{
    if (title == "") return false;
    for (char ch : "/\\,'`#$;!@\"*&?^:%-+=")
        if (title.find(ch) != std::string::npos)
            return false;
    return true;
}

void ComplexSearchWindow::on_title_textChanged(const QString &arg1)
{
    QPalette palette = ui->title->palette();
    if (checkText(arg1.toStdString())){
        palette.setColor(QPalette::Text, Qt::black);
    }
    else {
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->title->setPalette(palette);
}

void ComplexSearchWindow::on_genre_textChanged(const QString &arg1)
{
    QPalette palette = ui->genre->palette();
    if (checkText(arg1.toStdString())){
        palette.setColor(QPalette::Text, Qt::black);
    }
    else {
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->genre->setPalette(palette);
}

void ComplexSearchWindow::on_date_textChanged(const QString &arg1)
{
    QPalette palette = ui->date->palette();
    try {
        Movie::checkDate(ui->date->text().toStdString());
        palette.setColor(QPalette::Text, Qt::black);
    } catch (...) {
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->date->setPalette(palette);
}

void ComplexSearchWindow::on_min_rating_textChanged(const QString &arg1)
{
    QPalette palette = ui->min_rating->palette();
    string num_str = ui->min_rating->text().toStdString();
    if (num_str.find(',') != string::npos){
        palette.setColor(QPalette::Text, Qt::red);
    } else {
        try {
            stod(num_str);
            palette.setColor(QPalette::Text, Qt::black);
        } catch (...) {
            palette.setColor(QPalette::Text, Qt::red);
        }
    }
    ui->min_rating->setPalette(palette);
}

void ComplexSearchWindow::on_max_rating_textChanged(const QString &arg1)
{
    QPalette palette = ui->max_rating->palette();
    string num_str = ui->max_rating->text().toStdString();
    if (num_str.find(',') != string::npos){
        palette.setColor(QPalette::Text, Qt::red);
    } else {
        try {
            stod(num_str);
            palette.setColor(QPalette::Text, Qt::black);
        } catch (...) {
            palette.setColor(QPalette::Text, Qt::red);
        }
    }
    ui->max_rating->setPalette(palette);
}

void ComplexSearchWindow::on_startButton_clicked()
{
    string title = ui->title->text().toStdString();
    string genre = ui->genre->text().toStdString();
    string date = ui->date->text().toStdString();
    string min_rating_str = ui->min_rating->text().toStdString();
    string max_rating_str = ui->max_rating->text().toStdString();


    if (title != "" && !checkText(ui->title->text().toStdString())){
        QMessageBox::warning(this, "Создание фильтра", "Перепроверьте название.");
        return;
    }

    if (genre != "" && !checkText(ui->genre->text().toStdString())){
        QMessageBox::warning(this, "Создание фильтра", "Перепроверьте жанр.");
        return;
    }

    if (date != "") {
        try {
            Movie::checkDate(ui->date->text().toStdString());
        } catch (...) {
            QMessageBox::warning(this, "Создание фильтра", "Перепроверьте дату.");
            return;
        }
    }

    if (min_rating_str != ""){
        try {
            stod(min_rating_str);
        } catch (...) {
            QMessageBox::warning(this, "Создание фильтра", "Перепроверьте минимальный рейтинг.");
        }
    }
    if (max_rating_str != ""){
        try {
            stod(max_rating_str);
        } catch (...) {
            QMessageBox::warning(this, "Создание фильтра", "Перепроверьте максимальный рейтинг.");
        }
    }


    double min_rating;
    if (min_rating_str == "")
        min_rating = -1.;
    else
        min_rating = stod(min_rating_str);

    double max_rating;
    if (max_rating_str == "")
        max_rating = -1.;
    else
        max_rating = stod(max_rating_str);

    if (min_rating_str != "" && max_rating_str == ""){
        max_rating = 10.;
    } else if (min_rating_str == "" && max_rating_str != ""){
        min_rating = 0.;
    } else if (min_rating_str != "" && max_rating_str != "" && min_rating > max_rating){
        QMessageBox::warning(this, "Создание фильтра", "Минимум рейтинга не может превышать максимума.");
        return;
    }


    searchArgs->title = title;
    searchArgs->genre = genre;
    searchArgs->release_date = date;
    searchArgs->min_rating = min_rating;
    searchArgs->max_rating = max_rating;
    QMessageBox::information(this, "Создание фильтра", "Фильтр фильмов успешно создан.");
    close();
}
