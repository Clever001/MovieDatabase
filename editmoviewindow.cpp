#include "editmoviewindow.h"
#include "ui_editmoviewindow.h"
#include <QFileDialog>
#include <QMessageBox>

EditMovieWindow::EditMovieWindow(QWidget *parent, Movie *curMovie, MoviesManager *moviesManager) :
    QDialog(parent),
    ui(new Ui::EditMovieWindow),
    curMovie(curMovie),
    manager(moviesManager),
    isEdited(false)
{
    ui->setupUi(this);
    if (curMovie == nullptr){
        close();
    }
    ui->editTitle->setText(QString::fromStdString(curMovie->title));
    ui->editDate->setText(QString::fromStdString(curMovie->release_date));

    oldPosterName = curMovie->poster;

    for (const string& genre : curMovie->genres){
        genres.push_back(genre);
        ui->editGenresList->addItem(QString::fromStdString(genre));
    }

    ui->editRating->setText(QString::fromStdString(to_string(curMovie->rating)));
    ui->editDescription->setText(QString::fromStdString(curMovie->description));

    QString pixPath = QDir::currentPath() + "/data/posters/" + QString::fromStdString(curMovie->poster);
    if (QFile::exists(pixPath)){

        QPixmap pixmap(pixPath);
        int h = ui->poster->height();
        int w = ui->poster->width();
        ui->poster->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));
    } else {
        ui->poster->setText("Не был найден плакат.");
    }
}

EditMovieWindow::~EditMovieWindow()
{
    delete ui;
    if (!isEdited && newPosterName != "")
        deleteNewPoster();
}

bool EditMovieWindow::checkText(const string &title) const{
    if (title == "") return false;
    for (char ch : "/\\,'`#$;!@\"*&?^:%-+=")
        if (title.find(ch) != std::string::npos)
            return false;
    return true;
}

void EditMovieWindow::on_editTitle_textChanged(const QString &arg1)
{
    QPalette palette = ui->editTitle->palette();
    if (checkText(arg1.toStdString())){
        palette.setColor(QPalette::Text, Qt::black);
    }
    else {
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->editTitle->setPalette(palette);
}

void EditMovieWindow::on_editDate_textChanged(const QString &arg1)
{
    QPalette palette = ui->editDate->palette();
    try {
        Movie::checkDate(arg1.toStdString());
        palette.setColor(QPalette::Text, Qt::black);
    } catch (...) {
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->editDate->setPalette(palette);
}

void EditMovieWindow::on_editGenre_textChanged(const QString &arg1)
{
    QPalette palette = ui->editGenre->palette();
    if (checkText(arg1.toStdString())){
        palette.setColor(QPalette::Text, Qt::black);
    }
    else {
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->editGenre->setPalette(palette);
}

void EditMovieWindow::on_addGenreButton_clicked()
{
    QString genre = ui->editGenre->text();
    if (checkText(genre.toStdString()) && std::find(genres.begin(), genres.end(), genre.toStdString()) == genres.end()){
        genres.push_back(genre.toStdString());
        ui->editGenresList->addItem(genre);
        ui->editGenre->setText(QString::fromStdString(""));
    }
    else {
        QMessageBox::warning(this, "Неправильный жанр.", "Перепроверьте правильно ли у вас записан жанр. В нем не должны встречаться спец. символы. Также он должен быть уникальным.");
    }
}

void EditMovieWindow::on_editGenresList_itemDoubleClicked(QListWidgetItem *item)
{
    auto iter = std::find(genres.begin(), genres.end(), item->text().toStdString());
    if (iter == genres.end()){
        QMessageBox::warning(this, "Ошибка", "Удаляемый жанр фильма не был найден.");
        close();
    }
    genres.erase(iter);
    delete ui->editGenresList->takeItem(ui->editGenresList->row(item));
}

bool EditMovieWindow::checkRating(const string &rating) const {
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

void EditMovieWindow::on_editRating_textChanged(const QString &arg1)
{
    QPalette palette = ui->editRating->palette();
    if (checkRating(arg1.toStdString())){
        palette.setColor(QPalette::Text, Qt::black);
    } else {
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->editRating->setPalette(palette);
}

void EditMovieWindow::deleteOldPoster(){
    const QString savePath = QDir::currentPath() + "/data/posters/";
    if (oldPosterName != "" && QFile::exists(savePath + QString::fromStdString(oldPosterName))){
        QFile::remove(savePath + QString::fromStdString(oldPosterName));
        QPixmap pix;
        ui->poster->setPixmap(pix);
    }
}

void EditMovieWindow::deleteNewPoster(){
    const QString savePath = QDir::currentPath() + "/data/posters/";
    if (newPosterName != "" && QFile::exists(savePath + QString::fromStdString(newPosterName))){
        QFile::remove(savePath + QString::fromStdString(newPosterName));
    }
}

void EditMovieWindow::on_loadPosterButton_clicked()
{
    const QString posterPath = QFileDialog::getOpenFileName(this, "Выберите изображение", "", "Images (*.png *.jpg)");
    if (posterPath.isEmpty()) {
        QMessageBox::warning(this, "Обработка изображения", "При загрузке изображения возникла ошибка!");
        return;
    }

    deleteNewPoster();

    const QString savePath = QDir::currentPath() + "/data/posters/";
    QDir().mkpath(savePath);
    QString fileName = QFileInfo(posterPath).fileName();
    while (manager->duplicatePoster(fileName.toStdString())){
        fileName = QString::fromStdString("_") + fileName;
    }
    QString fullSavePath = savePath + fileName;


    if (QFile::copy(posterPath, fullSavePath)){
        newPosterName = fileName.toStdString();
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

void EditMovieWindow::on_editMovieButton_clicked()
{
    string title = ui->editTitle->text().toStdString();
    string date = ui->editDate->text().toStdString();
    // genres
    // posterName
    string rating_str = ui->editRating->text().toStdString();
    string description = ui->editDescription->toPlainText().toStdString();

    if (!checkText(title)){
        QMessageBox::warning(this, "Изменение фильма", "Перепроверьте заголовок.");
        return;
    }

    try {
        Movie::checkDate(date);
    } catch (...) {
        QMessageBox::warning(this, "Изменение фильма", "Перепроверьте дату выхода в прокат.");
        return;
    }

    if (genres.empty()){
        QMessageBox::warning(this, "Изменение фильма", "Добавьте хотя бы один жанр.");
        return;
    }

    for (const auto& genre : genres)
        if (genre == ""){
            QMessageBox::warning(this, "Изменение фильма", "Жанры не могут быть пустыми.");
            return;
        }

    double rating;
    if (checkRating(rating_str)){
        rating = stod(rating_str);
    } else {
        QMessageBox::warning(this, "Изменение фильма", "Неправильный рейтинг.");
        return;
    }

    /*
    if (manager->duplicateTitleAndDate(title, date)) {
        QMessageBox::warning(this, "Изменение фильма", "Фильм с таким названием и датой выхода в прокат уже создан. "
                                                       "Так как оба эти поля должны быть взаимно уникальны поменяйте название фильма или "
                                                       "дату выхода в прокат.");
        return;
    }
       */

    try {
        if (newPosterName != "")
            deleteOldPoster();

        *curMovie = Movie(title, (newPosterName == "") ? oldPosterName : newPosterName, date, vector<string>(genres.begin(), genres.end()), rating, description);
        isEdited = true;

        QMessageBox::warning(this, "Изменение фильма", "Фильм успешно сохранен!");
    } catch (...) {
        isEdited = false;
        QMessageBox::warning(this, "Изменение фильма", "Произошла неизвестная ошибка при создании фильма.");
        return;
    }
    close();
}
