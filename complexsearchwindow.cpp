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
bool ComplexSearchWindow::checkText(const string &text) const{
    // Типовой валидатор для текстового поля.
    if (text == "") return false;
    for (char ch : "/\\,'`#$;!@\"*&?^:%-+=")
        if (text.find(ch) != std::string::npos)
            return false;
    return true;
}

void ComplexSearchWindow::on_title_textChanged(const QString &arg1)
{
    // Здесь происходит окраска текста в красный в случае неудачной валидации.
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
    // Здесь происходит окраска текста в красный в случае неудачной валидации.
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
    // Здесь происходит окраска текста в красный в случае неудачной валидации.
    QPalette palette = ui->date->palette();
    try {
        Movie::checkDate(ui->date->text().toStdString()); // Проверка даты.
        palette.setColor(QPalette::Text, Qt::black);
        auto parsed_rating = Movie::parseDate(arg1.toStdString());
        string rating_str;
        for (int i = 0; i != 3; ++i){
            rating_str += to_string(parsed_rating[i]);
            if (i != 2) rating_str += '.';
        }
        ui->date->setText(QString::fromStdString(rating_str));
    } catch (...) {
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->date->setPalette(palette);
}

void ComplexSearchWindow::on_min_rating_textChanged(const QString &arg1)
{
    // Проверка рейтинга на наличие запятых. Также проверяется будет ли выбрасываться ошибка при приведении типов.
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
    // Проверка рейтинга на наличие запятых. Также проверяется будет ли выбрасываться ошибка при приведении типов.
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
    // Далее описывается процесс создания фильтра поиска.

    // Инициализация переменных для большего удобства.
    string title = ui->title->text().toStdString();
    string genre = ui->genre->text().toStdString();
    string date = ui->date->text().toStdString();
    string min_rating_str = ui->min_rating->text().toStdString();
    string max_rating_str = ui->max_rating->text().toStdString();


    // Проверка переменных на валидность.
    if (title != "" && !checkText(title)){
        QMessageBox::warning(this, "Создание фильтра", "Перепроверьте название.");
        return;
    }

    if (genre != "" && !checkText(genre)){
        QMessageBox::warning(this, "Создание фильтра", "Перепроверьте жанр.");
        return;
    }

    if (date != "") {
        try {
            Movie::checkDate(date);
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
            return;
        }
    }
    if (max_rating_str != ""){
        try {
            stod(max_rating_str);
        } catch (...) {
            QMessageBox::warning(this, "Создание фильтра", "Перепроверьте максимальный рейтинг.");
            return;
        }
    }

    // Инициализация минимального и максимального рейтинга.
    double min_rating = (min_rating_str == "") ? -1. : stod(min_rating_str);
    double max_rating = (max_rating_str == "") ? -1. : stod(max_rating_str);

    /* В следующем фрагменте кода проверяется был ла какой-либо рейтинг пустым.
     * Если было пусто одно из полей, то второму устанавливается крайнее значение и наоборот.
     * Если оба поля не были заданы, то не происходит никаких действий.
     * Если были заданы оба поля и минимальный рейтинг был больше максимального, то выводится ошибка.
     */
    if (min_rating_str != "" && max_rating_str == ""){
        max_rating = 10.;
    } else if (min_rating_str == "" && max_rating_str != ""){
        min_rating = 0.;
    } else if (min_rating_str != "" && max_rating_str != "" && min_rating > max_rating){
        QMessageBox::warning(this, "Создание фильтра", "Минимум рейтинга не может превышать максимума.");
        return;
    }

    // Полям фильтра поиска устанавливаются свои валидные значения. Окно закрывается.
    searchArgs->title = title;
    searchArgs->genre = genre;
    searchArgs->release_date = date;
    searchArgs->min_rating = min_rating;
    searchArgs->max_rating = max_rating;
    QMessageBox::information(this, "Создание фильтра", "Фильтр фильмов успешно создан.");
    close();
}
