#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newmoviewindow.h"
#include "editmoviewindow.h"
#include "complexsearchwindow.h"
#include <vector>
#include <QMessageBox>
#include <QFileDialog>
#include <QDate>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Здесь инициализируются все необходимые переменные для работы.
    ui->setupUi(this);

    dataPath = QDir::currentPath() + "/data/";
    moviesOutput.push_back(MovieOutput(ui->movie1Label, ui->movie1Date, ui->movie1Poster));
    moviesOutput.push_back(MovieOutput(ui->movie2Label, ui->movie2Date, ui->movie2Poster));
    moviesOutput.push_back(MovieOutput(ui->movie3Label, ui->movie3Date, ui->movie3Poster));
    moviesOutput.push_back(MovieOutput(ui->movie4Label, ui->movie4Date, ui->movie4Poster));
    manager = new MoviesManager();
    movies = manager->search();
    changeOffset(0);
    printMovies();
    ui->statusBar->showMessage("Приложение запущено успешно!");
}

MainWindow::~MainWindow()
{
    delete manager;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // Отображение диалогового окна с вопросом о сохранении.
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Закрытие приложения", "Вы хотите сохранить изменения?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Вызов действия сохранения
        ui->saveAction->trigger();
    }
}

void MainWindow::changeOffset(size_t val){
    // Изменение отступа, совмещенное с изменением информации на форме.
    offset = val;
    ui->offsetLabel->setText("Отступ: " + QString::fromStdString(to_string(val)));
}

void MainWindow::printMovies() const {
    // Выводятся 4 фильма в область предпросмотра с учетом текущего отступа.
    size_t cnt = movies.size();
    for (size_t i = 0; i != 4; ++i) {
        if (i + offset < cnt){
            moviesOutput[i].label->setText(QString::fromStdString(movies[i + offset]->title));
            moviesOutput[i].date->setText(QString::fromStdString(movies[i + offset]->release_date));
            QString curPos = dataPath + "posters/" + QString::fromStdString(movies[i + offset]->poster);
            if (QFile::exists(curPos)){
                QPixmap pixmap(curPos);
                int h = moviesOutput[i].poster->height();
                int w = moviesOutput[i].poster->width();
                moviesOutput[i].poster->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
            } else { // Постер не был найден.
                moviesOutput[i].poster->clear();
                moviesOutput[i].poster->setText("Картинка не была найдена.");
            }
        }
        else {
            moviesOutput[i].label->setText(QString::fromStdString("Нет фильма"));
            moviesOutput[i].date->setText(QString::fromStdString("Нет фильма"));
            moviesOutput[i].poster->clear();
            moviesOutput[i].poster->setText("Нет фильма");
        }
    }
}

void MainWindow::on_newFilmAction_triggered()
{
    // Открывается окно создания нового фильма.
    // После происходит проверка на изменение количества фильмов.
    size_t old_size = manager->size();
    NewMovieWindow window(this, manager);
    window.exec();
    if (old_size + 1 == manager->size()) {
        movies = manager->search();
        printMovies();
        ui->statusBar->showMessage("Был добавлен новый фильм.");
    }
    else if (old_size == manager->size())
        ui->statusBar->showMessage("Новый фильм не был добавлен.");
    else {
        // Программа никогда сюда не зайдет.
        QMessageBox::warning(this, "Ошибка","Ошибка в работе приложения.");
        close();
    }
}

void MainWindow::editMovie(const Movie *curMovie) {
    // Открывается окно редактирования конкретного фильма.
    if (curMovie == nullptr){
        QMessageBox::warning(this, "Изменение фильма", "Такого фильма нет.");
        return;
    }
    Movie tmp = *curMovie;
    EditMovieWindow window(this, &tmp, manager); // Окно взаимодействует с копией.
    window.exec();
    if (tmp == *curMovie){ // Фильм не был изменен.
        ui->statusBar->showMessage("Фильм не был изменен.");
    }
    else if (manager->editMovie(curMovie->title, curMovie->release_date, tmp)){ // Изменения фильма в списке.
        movies = manager->search();
        changeOffset(0);
        printMovies();
        ui->statusBar->showMessage("Фильм был успешно изменен.");
    }
    else { // Измененный фильм не был науден в списке.
        // Программа никогда сюда не зайдет (если все идет по плану).
        QMessageBox::warning(this, "Ошибка", "Ошибка в работе приложения.");
        close();
    }
}

void MainWindow::on_movie1EditButton_clicked()
{
    // Редактирование фильма.
    if (offset < movies.size())
        editMovie(movies[offset]);
    else
        editMovie(nullptr); // Произойдет вывод ошибки.
}

void MainWindow::on_movie2EditButton_clicked()
{
    // Редактирование фильма.
    if (offset + 1 < movies.size())
        editMovie(movies[offset + 1]);
    else
        editMovie(nullptr);
}


void MainWindow::on_movie3EditButton_clicked()
{
    // Редактирование фильма.
    if (offset + 2 < movies.size())
        editMovie(movies[offset + 2]);
    else
        editMovie(nullptr);
}

void MainWindow::on_movie4EditButton_clicked()
{
    // Редактирование фильма.
    if (offset + 3 < movies.size())
        editMovie(movies[offset + 3]);
    else
        editMovie(nullptr);
}

void MainWindow::on_complexSearchButton_clicked()
{
    // Поиск фильмов.
    SearchArgs searchArgs; // Фильтр поиска.
    ComplexSearchWindow window(this, &searchArgs);
    window.exec();

    // Далее ищутся фильмы по фильтру.
    movies = manager->search(searchArgs.title, searchArgs.genre, searchArgs.release_date, searchArgs.min_rating, searchArgs.max_rating);
    changeOffset(0);
    printMovies();
    ui->statusBar->showMessage("Фильтр поиска успешно изменен!");
}

void MainWindow::on_saveAction_triggered()
{
    // Обработчик события сохранения информации.
    QString savePath = dataPath + "json/";
    QDir().mkpath(savePath);
    savePath += "data.json";
    try {
        manager->saveToDb(savePath.toStdString());
        QMessageBox::information(this, "Сохранение информации", "Информация успешно сохранена.");
        ui->statusBar->showMessage("Информация успешно сохранена.");
    } catch (...) {
        QMessageBox::warning(this, "Сохранение информации", "Возникла ошибка при сохранении информации.");
        ui->statusBar->showMessage("Информация не была сохранена.");
    }
}

void MainWindow::on_loadAction_triggered()
{
    // Обработчик события загрузки информации.
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Получение информации", "Все текущие данные будут потеряны.\n\nВы точно хотите загрузить информацию из БД?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) return;

    const QString savePath = dataPath + "json/data.json";
    if (QFile::exists(savePath)){
        try {
            manager->loadFromDb(savePath.toStdString());
            movies = manager->search(); // Получение указателей на текущие фильмы. Нужно для работы области предпросмотра.
            changeOffset(0);
            printMovies(); // Вывод фильмов.
            QMessageBox::information(this, "Получение информации", "Информация успешно загружена с диска.");
            ui->statusBar->showMessage("Информация загружена с диска.");
        } catch (...) {
            QMessageBox::warning(this, "Получение информации", "Возникла ошибка при получении информации.");
            ui->statusBar->showMessage("Возникла ошибка при загрузке информации с диска.");
        }
    } else {
        QMessageBox::information(this, "Получение информации", "Возникла ошибка при получении информации. Убедитесь, была ли сохранена информация ранее.");
        ui->statusBar->showMessage("Возникла ошибка при загрузке информации с диска.");
    }
}

void MainWindow::on_nextButton_clicked()
{
    // Изменение отступа.
    if (offset < movies.size()) // Отступ валиден
        changeOffset(offset + 1);
    printMovies();
}

void MainWindow::on_previousButton_clicked()
{
    // Изменение отступа.
    if (offset > 0) // Отступ валиден
        changeOffset(offset - 1);
    printMovies();
}

void MainWindow::deleteMovie(size_t index) {
    // Удаление фильма.
    if (offset + index < movies.size()){
        QString posterPath = dataPath + "posters/" + QString::fromStdString(movies[offset + index]->poster);
        if (QFile::exists(posterPath))
            QFile::remove(posterPath);
        manager->removeMovie(movies[offset + index]->title, movies[offset + index]->release_date);
        movies = manager->search(); // вывод всех фильмов.
        changeOffset(0);
        printMovies();
        QMessageBox::information(this, "Удаление фильма", "Фильм успешно удален.");
        ui->statusBar->showMessage("Фильм успешно удален.");
    } else { // Такого фильма нет.
        QMessageBox::warning(this, "Удаление фильма", "Такого фильма нет.");
        ui->statusBar->showMessage("Такого фильма нет.");
    }
}

void MainWindow::on_movie1DelButton_clicked()
{
    deleteMovie(0);
}

void MainWindow::on_movie2DelButton_clicked()
{
    deleteMovie(1);
}

void MainWindow::on_movie3DelButton_clicked()
{
    deleteMovie(2);
}

void MainWindow::on_movie4DelButton_clicked()
{
    deleteMovie(3);
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    // Поиск фильмов по названию.
    movies = manager->search(ui->lineEdit->text().toStdString());
    changeOffset(0);
    printMovies();
    if (arg1.isEmpty()){
        ui->statusBar->showMessage("Вывод всех фильмов.");
    } else {
        ui->statusBar->showMessage("Вывод фильмов с названиме, похожим на \"" + arg1 + "\".");
    }
}

void MainWindow::on_actionnotReleasedAction_triggered()
{
    // Показ еще не вышедших в показ фильмов.
    movies = manager->search(QDate::currentDate().toString("dd.MM.yyyy").toStdString(), Compare::More);
    changeOffset(0);
    printMovies();
    ui->statusBar->showMessage("Показаны еще не выпущенные в прокат фильмы.");
}

void MainWindow::on_viewAllFilmsAction_triggered()
{
    // Показ всех фильмов.
    movies = manager->search();
    changeOffset(0);
    printMovies();
    ui->statusBar->showMessage("Вывод всех фильмов.");
}

void MainWindow::on_aboutAction_triggered()
{
    // Показ окна "О программе".
    QMessageBox::about(this, "О программе", "Разработчик программы: студент группы КЭ-243 Никончук Виталий. "
                                                  "\n\nПрограмма предназначена для работы с информацией о фильмах. Проект предоставляет возможность сохранения, "
                                                  "изменения и просмотра информации о фильмах. Информацию можно сохранять в файле формата json. "
                                                  "\n\nЮУрГУ Июль 2024.");
    QMessageBox::aboutQt(this, "О платформе"); // Вывод информации о Qt.
}

void MainWindow::on_helpAction_triggered()
{
    // Показ информации о помощи.
    QMessageBox::information(this, "Помощь", "<h2>Создание фильма</h2><p>Для создания фильма выберите пункт \"Фильмы->Создать новый фильм\" в основном окне. Далее нужно заполнить поля формы. "
                             "Каждое поле подсвечивается красным, если оно было заполнено не валидными данными. Если вы что-то сделали не так при заполнении информации о фильме"
                             ", то программа об этом сообщит. Для удаления некоректных жанров просто нажмите на него два раза.</p>"
                             "<h2>Просмотр/изменение информации о фильме</h2><p>В основном окне нажмите кнопку \"info\" под требуемым фильмом. Измените информацию так, как вы пожелаете. "
                             "Для сохранения изменений нажмите изменить фильм, иначе просто закройте диалоговое окно.</p>"
                             "<h2>Удаление фильма</h2><p>В основном окне нажмите кнопку \"Удалить\" под удаляемым фильмом.</p>"
                             "<h2>Установка фильтра поиска</h2><p>В основном окне нажмите \"Использовать сложный поиск\". Заполните необходимые поля (все поля необязательные к заполнению). "
                             "Примените фильтр.</p>"
                             "<h2>Поиск не вышедших фильмов</h2><p>Для поиска невышедших фильмов выберите пункт \"Фильмы->Поиск невышедших фильмов\" в основном окне программы.</p>"
                             "<h2>Сортировка</h2><p>В пункте \"Фильмы->Сортировка\" есть несколько подпунктов, выбрав которые можно отсортировать фильмы, находящиеся в главном окне.</p>"
                             "<h2>Сохранение/загрузка изменений</h2><p>Нажимайте соответствующие кнопки меню в пункте \"Файл\".</p>");
}

void MainWindow::on_sortByTitleAction_triggered()
{
    // Сортировка по названию.
    sort(movies.begin(), movies.end(), [](const Movie* lhs, const Movie* rhs){
        return lhs->title < rhs->title;
    });
    changeOffset(0);
    printMovies();
}

void MainWindow::on_sortByDateAction_triggered()
{
    // Сортировка по дате выхода в прокат.
    sort(movies.rbegin(), movies.rend(), [](const Movie* lhs, const Movie* rhs){
        array<int, 3> parsed_lhs = Movie::parseDate(lhs->release_date);
        array<int, 3> parsed_rhs = Movie::parseDate(rhs->release_date);
        return tie(parsed_lhs[2], parsed_lhs[1], parsed_lhs[0]) < tie(parsed_rhs[2], parsed_rhs[1], parsed_rhs[0]);
    });
    changeOffset(0);
    printMovies();
}

void MainWindow::on_sortByRatingAction_triggered()
{
    // Сортировка по рейтингу.
    sort(movies.rbegin(), movies.rend(), [](const Movie* lhs, const Movie* rhs){
        return lhs->rating < rhs->rating;
    });
    changeOffset(0);
    printMovies();
}
