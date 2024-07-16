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
    // Начальная инициализация значений, вывод информации о фильме в окно.
    ui->editTitle->setText(QString::fromStdString(curMovie->title));
    ui->editDate->setText(QString::fromStdString(curMovie->release_date));

    oldPosterName = curMovie->poster;

    originalName = curMovie->title;
    originalDate = curMovie->release_date;

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
        deleteNewPoster(); // Очистка постера, так как пользователь отменил изменения.
}

bool EditMovieWindow::checkText(const string &text) const{
    // Стандартная валидация текстового поля.
    if (text == "") return false;
    for (char ch : "/\\,'`#$;!@\"*&?^:%-+=")
        if (text.find(ch) != std::string::npos)
            return false;
    return true;
}

void EditMovieWindow::on_editTitle_textChanged(const QString &arg1)
{
    // Здесь происходит окраска текста в красный в случае неудачной валидации.
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
    // Здесь происходит окраска текста в красный в случае неудачной валидации.
    QPalette palette = ui->editDate->palette();
    try {
        Movie::checkDate(arg1.toStdString()); // Проверка даты на валидность.
        palette.setColor(QPalette::Text, Qt::black);
    } catch (...) {
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->editDate->setPalette(palette);
}

void EditMovieWindow::on_editGenre_textChanged(const QString &arg1)
{
    // Здесь происходит окраска текста в красный в случае неудачной валидации.
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
    // Добавление нового жанра. Также проверяется уникальность и валидность жанра.
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
    // Удаление выбранного жанра.
    auto iter = std::find(genres.begin(), genres.end(), item->text().toStdString());
    if (iter == genres.end()){
        QMessageBox::warning(this, "Ошибка", "Удаляемый жанр фильма не был найден.");
        close();
    }
    genres.erase(iter);
    delete ui->editGenresList->takeItem(ui->editGenresList->row(item));
}

bool EditMovieWindow::checkRating(const string &rating) const {
    // Метод, служащий для проверки рейтинга.
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
    // Здесь происходит окраска текста в красный в случае неудачной валидации.
    QPalette palette = ui->editRating->palette();
    palette.setColor(QPalette::Text, (checkRating(arg1.toStdString())) ? Qt::black : Qt::red);
    ui->editRating->setPalette(palette);
}

void EditMovieWindow::deleteOldPoster(){
    // Удаление старого постера.
    const QString savePath = QDir::currentPath() + "/data/posters/";
    if (oldPosterName != "" && QFile::exists(savePath + QString::fromStdString(oldPosterName))){
        QFile::remove(savePath + QString::fromStdString(oldPosterName));
        QPixmap pix;
        ui->poster->setPixmap(pix);
    }
}

void EditMovieWindow::deleteNewPoster(){
    // Удаление нового постера.
    const QString savePath = QDir::currentPath() + "/data/posters/";
    if (newPosterName != "" && QFile::exists(savePath + QString::fromStdString(newPosterName))){
        QFile::remove(savePath + QString::fromStdString(newPosterName));
    }
}

void EditMovieWindow::on_loadPosterButton_clicked()
{
    // Загрузка нового постера.
    const QString posterPath = QFileDialog::getOpenFileName(this, "Выберите изображение", "", "Images (*.png *.jpg)");
    if (posterPath.isEmpty()) {
        QMessageBox::warning(this, "Обработка изображения", "При загрузке изображения возникла ошибка!");
        return;
    }

    deleteNewPoster(); // Так как этот постер больше не нужен, он удаляется.

    const QString savePath = QDir::currentPath() + "/data/posters/";
    QDir().mkpath(savePath);
    QString fileName = QFileInfo(posterPath).fileName();
    while (manager->duplicatePoster(fileName.toStdString())){ // Проверяется был ли до этого уже создан постер с таким названием.
        fileName = QString::fromStdString("_") + fileName;
    }
    const QString fullSavePath = savePath + fileName;


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
    // Редактирование фильма.

    // Инициализация значений для большего удобства.
    string title = ui->editTitle->text().toStdString();
    string date = ui->editDate->text().toStdString();
    // genres
    // posterName
    string rating_str = ui->editRating->text().toStdString();
    string description = ui->editDescription->toPlainText().toStdString();

    // Проверка всех данных на валидность.
    if (!checkText(title)) {
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

    // Если была изменена дата или название, то их нужно проверить на валидность.
    if (title != originalName || date != originalDate){
        if (manager->duplicateTitleAndDate(title, date)){
            QMessageBox::warning(this, "Изменение фильма", "Фильм с таким названием и датой выхода в прокат уже был создан. "
                                                           "Пожалуйста, измените название фильма или дату выхода в прокат.");
            return;
        }
    }

    // Далее создается новый фильм. Сам фильм будет заменять существующий в главном окне.
    try {
        // Создание нового фильма. Удаление старого постера, если был назначен новый.
        if (newPosterName != "")
            deleteOldPoster();

        *curMovie = Movie(title, (newPosterName == "") ? oldPosterName : newPosterName, date, vector<string>(genres.begin(), genres.end()), rating, description);
        isEdited = true;

        QMessageBox::information(this, "Изменение фильма", "Фильм успешно сохранен!");
    } catch (...) {
        // Программа никогда сюда не зайдет!
        isEdited = false;
        QMessageBox::warning(this, "Изменение фильма", "Произошла неизвестная ошибка при создании фильма.");
        return;
    }
    close();
}
