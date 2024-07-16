#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include "MovieModel.h"
#include <algorithm>
#include <cctype>
#include <codecvt>
#include <fstream>
#include <iterator>
#include <json.hpp>
#include <stdexcept>
#include <string>
#include <QString>

using json = nlohmann::json;

Movie::Movie(const string& title, const string& poster, const string& date,
    const vector<string>& genres, double rating, const string& description)
    : title(title), poster(poster), release_date(date), genres(genres), rating(rating),
    description(description) {
    /* При переходе в тело конструктора все поля уже проинициализированы.
     * Теперь лишь нужно проверит. все поля на пустоту и валидность.
     * Все исключения помечены текстом, чтобы можно было понять в чем проблема.
     */
    if (title == "") throw length_error("Empty field: title.");
    if (poster == "") throw length_error("Empty field: poster.");
    Movie::checkDate(date);
    for (const string& genre : genres)
        if (genre == "") throw length_error("Empty field: genre.");
    if (rating < 0. || rating > 10.) throw invalid_argument("Invalid field: rating.");
    // description can be empty.
}

array<int, 3> Movie::parseDate(const string& date) {
    /* Разбиение даты на дань, месяц и год.
     */
    array<int, 3> parsed_date{};
    int cur = 0;
    string buf;
    for (char ch : date) {
        buf += ch;
        if (ch == '.') {
            if (cur > 1 || buf == "") throw invalid_argument("Invalid field: date.");
            try {
                parsed_date[cur] = stoi(buf);
            }
            catch (const invalid_argument& ex) {
                throw invalid_argument("Invalid field: date.");
            }
            buf.clear();
            ++cur;
        }
    }
    if (cur != 2) throw invalid_argument("Invalid field: date.");
    parsed_date[cur] = stoi(buf);
    return parsed_date;
}

bool Movie::isLeap(int year) {
    // Проверка года на високосность.
    if (!(year % 400)) return true;
    else if (!(year % 100)) return false;
    else if (!(year % 4)) return true;
    else return false;
}

int Movie::daysCount(int month, int year) {
    // Получение максимального числа дней в месяце.
    switch (month) {
    case 2:
        if (Movie::isLeap(year)) {
            return 29;
        }
        else {
            return 28;
        }
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        return 31;
    default:
        return 30;
    }
}

void Movie::checkDate(const string& date) {
    /* Проверка даты на валидность.
     * Если дата не валидна - выбрасывается исключение.
     */
    if (date == "") throw length_error("Empty field: date.");
    for (char ch : date){
        if (ch == ' ') throw invalid_argument("Invalid field: date.");
    }
    array<int, 3> parsed_date = Movie::parseDate(date);
    if (parsed_date[2] < 1900) throw invalid_argument("Not correct year");
    if (parsed_date[1] < 1 || parsed_date[1] > 12) throw invalid_argument("Not correct month.");
    int days_count = Movie::daysCount(parsed_date[1], parsed_date[2]);
    if (parsed_date[0] < 1 || parsed_date[0] > days_count) throw invalid_argument("Not correct day.");
}

bool Movie::operator==(const Movie& other) const {
    // Оператор сравнения двух объектов.
    return (
        this->title == other.title &&
        this->genres == other.genres &&
        this->poster == other.poster &&
        this->rating == other.rating &&
        this->description == other.description &&
        this->release_date == other.release_date
        );
}

bool MoviesManager::removeMovie(const string& title, const string& release_date) {
    /* Удаление фильма.
     * Так как название и дата взаимно уникальны, по ним ведется поиск удаляемого фильма.
     */
    for (auto iter = this->begin(); iter != this->end(); iter++) {
        if (iter->title == title && iter->release_date == release_date) {
            this->erase(iter);
            return true;
        }
    }
    return false;
}

bool MoviesManager::editMovie(const string& title, const string& release_date, const Movie& movie) {
    // Редактирование фильма.
    auto iter = find_if(this->begin(), this->end(), [&title, &release_date](const Movie& item) {
        return item.title == title && item.release_date == release_date;
        });
    if (iter == this->end()) return false;
    *iter = movie;
    return true;
}



vector<const Movie*> MoviesManager::search(const string& title, const string& genre,
    const string& release_date, double min_rating, double max_rating) const {
    // Поиск нужных фильмов по нескольким ключам.

    if (release_date != "") Movie::checkDate(release_date);

    vector<const Movie*> ans;
    if (title == "" && genre == "" && release_date == "" && min_rating == -1.0 && max_rating == -1.0) {
        for (const Movie& item : *this) {
            ans.push_back(&item);
        }
        return ans;
    }

    for (const Movie& item : *this) {
        if ((title == "" || MoviesManager::containsWord(title, item.title)) &&
            (genre == "" || any_of(item.genres.begin(), item.genres.end(),
                [&genre](const string& item_genre) {return MoviesManager::containsWord(genre, item_genre); })) &&
            (release_date == "" || item.release_date == release_date) &&
            (min_rating == -1 && max_rating == -1 || item.rating >= min_rating && item.rating <= max_rating))
            ans.push_back(&item);
    }
    return ans;
}

vector<const Movie*> MoviesManager::search(const string& cur_date, Compare compare) const {
    // Данный метод нужен для поиска фильмов, вышедших до, после или в определенную дату.
    auto cmp = [compare](const array<int, 3>& p_item, const array<int, 3>& p_cur){
        if (compare == Compare::More) return tie(p_item[2], p_item[1], p_item[0]) > tie(p_cur[2], p_cur[1], p_cur[0]);
        if (compare == Compare::Less) return tie(p_item[2], p_item[1], p_item[0]) < tie(p_cur[2], p_cur[1], p_cur[0]);
        return tie(p_item[2], p_item[1], p_item[0]) == tie(p_cur[2], p_cur[1], p_cur[0]);
    };

    Movie::checkDate(cur_date);
    array<int, 3> p_cur = Movie::parseDate(cur_date);
    vector<const Movie*> ans;

    for (const Movie& item : *this) {
        array<int, 3> p_item = Movie::parseDate(item.release_date);
        if (cmp(p_item, p_cur))
            ans.push_back(&item);
    }

    return ans;
}

vector<int> MoviesManager::prefixFunction(const string& str) {
    /* Функция нужна для работы метода containsWord, находящего подстроку в строке.
     * Использование префиксной функции из алгоритма КМП (Кнута, Мориса и Пратта) здесь избыточно.
     */
    vector<int> pr(str.size());
    for (int i = 1; i != str.size(); ++i) {
        int j = pr[i - 1];
        while ((j > 0) && (str[i] != str[j]))
            j = pr[j - 1];
        if (str[i] == str[j])
            ++j;
        pr[i] = j;
    }
    return pr;
}


static void lower(string& input) {
    // Переводит строку в нижний регистр.
    /*
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    wstring wideStr = converter.from_bytes(input);

    transform(wideStr.begin(), wideStr.end(), wideStr.begin(), towlower);

    input = converter.to_bytes(wideStr);
    */
    QString str = QString::fromStdString(input);
    str = str.toLower();
    input = str.toStdString();
}

bool MoviesManager::containsWord(string word, string text) {
    // Проверяет слово на вхождение в текст.
    lower(word);
    lower(text);
    string workWord = word + '/' + text;
    vector<int> pr = MoviesManager::prefixFunction(workWord);
    for (int i = 0; i != pr.size(); ++i) {
        if (pr[i] == word.size()) {
            return true;
        }
    }
    return false;
}

void MoviesManager::loadFromDb(const string& db_path) {
    // Загружает информацию в список из файла json.
    ifstream file(db_path);
    if (!file.is_open()) {
        throw runtime_error("Could not open file for reading.");
    }

    json j;
    file >> j;
    file.close();

    this->clear();
    for (const auto& item : j) {
        Movie movie(
            item.at("title").get<std::string>(),
            item.at("poster").get<std::string>(),
            item.at("release_date").get<std::string>(),
            item.at("genres").get<std::vector<std::string>>(),
            item.at("rating").get<double>(),
            item.at("description").get<std::string>()
        );
        this->push_back(movie);
    }
}

void MoviesManager::saveToDb(const string& db_path) const {
    // Сохраняет информацию в файл json из списка.
    json j = json::array();
    for (const auto& movie : *this) {
        j.push_back({
            {"title", movie.title},
            {"poster", movie.poster},
            {"release_date", movie.release_date},
            {"genres", movie.genres},
            {"rating", movie.rating},
            {"description", movie.description}
            });
    }

    ofstream file(db_path);
    if (!file.is_open()) {
        throw runtime_error("Could not open file for writing.");
    }

    file << j.dump(4);
    file.close();
}

bool MoviesManager::duplicatePoster(const string& poster) const {
    // Проверяет постер на вхождение. Используется для сохранения уникальности постеров.
    for (auto iter = this->begin(); iter != this->end(); ++iter) {
        if (iter->poster == poster) return true;
    }
    return false;
}

bool MoviesManager::duplicateTitleAndDate(const string& title, const string& date) const {
    /* Проверяет на фхождения пары названия и даты.
     * Используется для сохранения взаимной уникальности данных полей.
     */
    for (auto iter = this->begin(); iter != this->end(); ++iter) {
        if (iter->title == title && iter->release_date == date)
            return true;
    }
    return false;
}
