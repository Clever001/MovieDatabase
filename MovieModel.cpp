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
    if (title == "") throw length_error("Empty field: title.");
    if (poster == "") throw length_error("Empty field: poster.");
    Movie::checkDate(date);
    for (const string& genre : genres)
        if (genre == "") throw length_error("Empty field: genre.");
    if (rating < 0. || rating > 10.) throw invalid_argument("Invalid field: rating.");
    // description can be empty.
}

array<int, 3> Movie::parseDate(const string& date) {
    array<int, 3> parsed_date{};
    int cur = 0;
    string buf;
    for (char ch : date) {
        buf += ch;
        if (ch == '.') {
            if (cur > 1 || buf == "") throw invalid_argument("Invalid field: date.");
            parsed_date[cur] = stoi(buf);
            buf.clear();
            ++cur;
        }
    }
    if (cur != 2) throw invalid_argument("Invalid field: date.");
    parsed_date[cur] = stoi(buf);
    return parsed_date;
}

bool Movie::isLeap(int year) {
    if (!(year % 400)) return true;
    else if (!(year % 100)) return false;
    else if (!(year % 4)) return true;
    else return false;
}

int Movie::daysCount(int month, int year) {
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
    if (date == "") throw length_error("Empty field: date.");
    array<int, 3> parsed_date = Movie::parseDate(date);
    if (parsed_date[2] < 1900) throw invalid_argument("Not correct year");
    if (parsed_date[1] < 1 || parsed_date[1] > 12) throw invalid_argument("Not correct month.");
    int days_count = Movie::daysCount(parsed_date[1], parsed_date[2]);
    if (parsed_date[0] < 1 || parsed_date[0] > days_count) throw invalid_argument("Not correct day.");
}

bool Movie::operator==(const Movie& other) const {
    return (
        this->title == other.title &&
        this->genres == other.genres &&
        this->poster == other.poster &&
        this->rating == other.rating &&
        this->description == other.description &&
        this->release_date == other.release_date
        );
}

void MoviesManager::addMovie(const Movie& movie) {
    movies.push_back(movie);
}

bool MoviesManager::removeMovie(const string& title, const string& release_date) {
    for (auto iter = movies.begin(); iter != movies.end(); iter++) {
        if (iter->title == title && iter->release_date == release_date) {
            movies.erase(iter);
            return true;
        }
    }
    return false;
}

bool MoviesManager::editMovie(const string& title, const string& release_date, const Movie& movie) {
    auto iter = find_if(movies.begin(), movies.end(), [&title, &release_date](const Movie& item) {
        return item.title == title && item.release_date == release_date;
        });
    if (iter == movies.end()) return false;
    *iter = movie;
    return true;
}

vector<const Movie*> MoviesManager::search(const string& title, const string& genre,
    const string& release_date, double min_rating, double max_rating) const {

    if (release_date != "") Movie::checkDate(release_date);

    vector<const Movie*> ans;
    if (title == "" && genre == "" && release_date == "" && min_rating == -1.0 && max_rating == -1.0) {
        for (const Movie& item : movies) {
            ans.push_back(&item);
        }
        return ans;
    }

    for (const Movie& item : movies) {
        if ((title == "" || MoviesManager::containsWord(title, item.title)) &&
            (genre == "" || any_of(item.genres.begin(), item.genres.end(),
                [&genre](const string& item_genre) {return MoviesManager::containsWord(genre, item_genre); })) &&
            (release_date == "" || item.release_date == release_date) &&
            (min_rating == -1 && max_rating == -1 || item.rating >= min_rating && item.rating <= max_rating))
            ans.push_back(&item);
    }
    return ans;
}

vector<const Movie*> MoviesManager::searchNotReleased(const string& cur_date) const {
    Movie::checkDate(cur_date);
    array<int, 3> p_cur = Movie::parseDate(cur_date);
    vector<const Movie*> ans;

    for (const Movie& item : movies) {
        array<int, 3> p_item = Movie::parseDate(item.release_date);
        if (tie(p_cur[2], p_cur[1], p_cur[0]) < tie(p_item[2], p_item[1], p_item[0]))
            ans.push_back(&item);
    }

    /*
    copy_if(movies.begin(), movies.end(), back_inserter(ans), [&p_cur](const Movie& item) {
        array<int, 3> p_item = Movie::parseDate(item.release_date);
        return tie(p_cur[2], p_cur[1], p_cur[0]) < tie(p_item[2], p_item[1], p_item[0]);
        });
    */
    return ans;
}

vector<int> MoviesManager::prefixFunction(const string& str) {
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
    ifstream file(db_path);
    if (!file.is_open()) {
        throw runtime_error("Could not open file for reading.");
    }

    json j;
    file >> j;
    file.close();

    movies.clear();
    for (const auto& item : j) {
        Movie movie(
            item.at("title").get<std::string>(),
            item.at("poster").get<std::string>(),
            item.at("release_date").get<std::string>(),
            item.at("genres").get<std::vector<std::string>>(),
            item.at("rating").get<double>(),
            item.at("description").get<std::string>()
        );
        movies.push_back(movie);
    }
}

void MoviesManager::saveToDb(const string& db_path) const {
    json j = json::array();
    for (const auto& movie : movies) {
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

size_t MoviesManager::size() const {
    return movies.size();
}

bool MoviesManager::empty() const {
    return movies.empty();
}

bool MoviesManager::duplicatePoster(const string& poster) const {
    for (auto iter = movies.begin(); iter != movies.end(); ++iter) {
        if (iter->poster == poster) return true;
    }
    return false;
}

bool MoviesManager::duplicateTitleAndDate(const string& title, const string& date) const {
    for (auto iter = movies.begin(); iter != movies.end(); ++iter) {
        if (iter->title == title && iter->release_date == date)
            return true;
    }
    return false;
}
