#include "newmoviewindow.h"
#include "ui_newmoviewindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QPixmap>

NewMovieWindow::NewMovieWindow(QWidget *parent, MoviesManager* m) :
    QDialog(parent),
    ui(new Ui::NewMovieWindow),
    manager(m),
    isSaved(false)
{
    ui->setupUi(this);
    ui->poster->setText(QString::fromStdString("Загрузите постер."));
}

NewMovieWindow::~NewMovieWindow()
{
    delete ui;
    if (!isSaved) { // Так как создание нового фильма было отменено, удаляется картинка постера.
        const QString savePath = QDir::currentPath() + "/data/posters/";
        if (posterName != "" && QFile::exists(savePath + QString::fromStdString(posterName))){
            QFile::remove(savePath + QString::fromStdString(posterName));
        }
    }
}

bool NewMovieWindow::checkText(const string &title) const{
    // Стандартная валидация текстового поля.
    if (title == "") return false;
    for (char ch : "/\\,'`#$;!@\"*&?^:%-+=")
        if (title.find(ch) != std::string::npos)
            return false;
    return true;
}

void NewMovieWindow::on_newTitle_textChanged(const QString &arg1)
{
    // Здесь происходит окраска текста в красный в случае неудачной валидации.
    QPalette palette = ui->newTitle->palette();
    if (checkText(arg1.toStdString())){
        palette.setColor(QPalette::Text, Qt::black);
    }
    else {
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->newTitle->setPalette(palette);
}

void NewMovieWindow::on_newDate_textChanged(const QString &arg1)
{
    // Здесь происходит окраска текста в красный в случае неудачной валидации.
    QPalette palette = ui->newDate->palette();
    try {
        Movie::checkDate(arg1.toStdString()); // Проверка даты.
        palette.setColor(QPalette::Text, Qt::black);
        auto parsed_rating = Movie::parseDate(arg1.toStdString());
        string rating_str;
        for (int i = 0; i != 3; ++i){
            rating_str += to_string(parsed_rating[i]);
            if (i != 2) rating_str += '.';
        }
        ui->newDate->setText(QString::fromStdString(rating_str));
    }
    catch (...) {
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->newDate->setPalette(palette);
}

void NewMovieWindow::on_newGenre_textChanged(const QString &arg1)
{
    // Здесь происходит окраска текста в красный в случае неудачной валидации.
    QPalette palette = ui->newGenre->palette();
    if (checkText(arg1.toStdString())){
        palette.setColor(QPalette::Text, Qt::black);
    }
    else {
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->newGenre->setPalette(palette);
}

void NewMovieWindow::on_newGenresList_itemDoubleClicked(QListWidgetItem *item)
{
    // Удаление выбранного жанра.
    auto iter = std::find(genres.begin(), genres.end(), item->text().toStdString());
    if (iter == genres.end()){
        QMessageBox::warning(this, "Ошибка", "Удаляемый жанр фильма не был найден.");
        close();
    }
    genres.erase(iter);
    delete ui->newGenresList->takeItem(ui->newGenresList->row(item));
}

void NewMovieWindow::on_addGenreButton_clicked()
{
    // Добавление нового жанра при условии его валидности и уникальности.
    QString genre = ui->newGenre->text();
    if (checkText(genre.toStdString()) && std::find(genres.begin(), genres.end(), genre.toStdString()) == genres.end()){
        genres.push_back(genre.toStdString());
        ui->newGenresList->addItem(genre);
        ui->newGenre->setText(QString::fromStdString(""));
    }
    else {
        QMessageBox::warning(this, "Неправильный жанр", "Перепроверьте правильно ли у вас записан жанр. В нем не должны встречаться спец. символы. Также он должен быть уникальным.");
    }
}

bool NewMovieWindow::checkRating(const string &rating) const {
    // Проверка рейтинга на наличие запятых и соответствие диапазоны [0, 10].
    double num;
    try {
        num = stod(rating);
    }
    catch (...){
        return false;
    }
    if (num < 0. || num > 10. || rating.find(',') != string::npos) return false;
    return true;
}

void NewMovieWindow::on_newRating_textChanged(const QString &arg1)
{
    // Здесь происходит окраска текста в красный в случае неудачной валидации.
    QPalette palette = ui->newRating->palette();
    palette.setColor(QPalette::Text, (checkRating(arg1.toStdString())) ? Qt::black : Qt::red);
    ui->newRating->setPalette(palette);
}

void NewMovieWindow::deleteCurPoster() {
    // Удаление текущего постера.
    const QString savePath = QDir::currentPath() + "/data/posters/";
    if (posterName != "" && QFile::exists(savePath + QString::fromStdString(posterName))){
        QFile::remove(savePath + QString::fromStdString(posterName));
        QPixmap pix;
        ui->poster->setPixmap(pix);
    }
}

void NewMovieWindow::on_loadPosterButton_clicked()
{
    // Загрузка нового постера.
    const QString posterPath = QFileDialog::getOpenFileName(this, "Выберите изображение", "", "Images (*.png *.jpg)");
    if (posterPath.isEmpty()) {
        QMessageBox::warning(this, "Обработка изображения", "При загрузке изображения возникла ошибка!");
        return;
    }

    deleteCurPoster();

    const QString savePath = QDir::currentPath() + "/data/posters/";
    QDir().mkpath(savePath);
    QString fileName = QFileInfo(posterPath).fileName();
    while (manager->duplicatePoster(fileName.toStdString())){ // Проверка названия на дупликацию.
        fileName = QString::fromStdString("_") + fileName;
    }
    QString fullSavePath = savePath + fileName;


    if (QFile::copy(posterPath, fullSavePath)){
        posterName = fileName.toStdString(); // Установка нового названия.
        int h = ui->poster->height();
        int w = ui->poster->width();
        QPixmap pixmap(fullSavePath);
        ui->poster->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));
        QMessageBox::information(this, "Обработка изображения", "Изображение успешно загружено!");
    }
    else {
        QMessageBox::warning(this, "Обработка изображения", "Не удалось загрузить изображение!");
    }
}

void NewMovieWindow::on_addMovieButton_clicked()
{
    // Создание нового фильма.

    // Инициализация переменных для удобства.
    string title = ui->newTitle->text().toStdString();
    string date = ui->newDate->text().toStdString();
    // genres
    // posterName
    string rating_str = ui->newRating->text().toStdString();
    string description = ui->newDescription->toPlainText().toStdString();

    // Валидация всех полей.
    if (!checkText(title)){
        QMessageBox::warning(this, "Создание фильма", "Перепроверьте заголовок.");
        return;
    }

    try {
        Movie::checkDate(date);
    } catch (...) {
        QMessageBox::warning(this, "Создание фильма", "Перепроверьте дату выхода в прокат.");
        return;
    }

    if (genres.empty()){
        QMessageBox::warning(this, "Создание фильма", "Добавьте хотя бы один жанр.");
        return;
    }

    for (const auto& genre : genres)
        if (genre == ""){
            QMessageBox::warning(this, "Создание фильма", "Жанры не могут быть пустыми.");
            return;
        }

    if (posterName == ""){
        QMessageBox::warning(this, "Создание фильма", "Загрузите плакат.");
        return;
    }

    double rating;
    if (checkRating(rating_str)){
        rating = stod(rating_str);
    } else {
        QMessageBox::warning(this, "Создание фильма", "Неправильный рейтинг.");
        return;
    }

    // Проверка названия и даты на валидность.
    if (manager->duplicateTitleAndDate(title, date)){
        QMessageBox::warning(this, "Создание фильма", "Фильм с таким названием и датой выхода в прокат уже создан. "
                                                      "Так как оба эти поля должны быть взаимно уникальны поменяйте название фильма или "
                                                      "дату выхода в прокат.");
        return;
    }


    // Создание нового фильма.
    try {
        Movie newMovie(title, posterName, date, vector<string>(genres.begin(), genres.end()), rating, description);
        manager->push_back(newMovie);
        isSaved = true;
        QMessageBox::information(this, "Создание фильма", "Фильм успешно сохранен!");
    } catch (...) {
        isSaved = false;
        QMessageBox::warning(this, "Создание фильма", "Произошла неизвестная ошибка при создании фильма.");
        return;
    }
    close();
}
