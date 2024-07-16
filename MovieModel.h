#pragma once
#include <array>
#include <list>
#include <string>
#include <vector>

using namespace std;

class Movie {
    /* Данный класс хранит информацию о фильмах.
     * Конструктор класса содержит в себе проверку на валидность данных.
     * Если информация не будет валидной, будет выброшено исключение.
     */
    static bool isLeap(int year);
    static int daysCount(int month, int year);
public:
    string title;
    string poster;
    string release_date;
    vector<string> genres;
    double rating;
    string description;

    Movie(const string& title, const string& poster, const string& date,
        const vector<string>& genres, double rating, const string& description);

    static array<int, 3> parseDate(const string& date);
    static void checkDate(const string& date);
    bool operator==(const Movie& other) const;
};

enum Compare {
    // Перечисление нужно для второй перегрузки метода MoviesManager::search.
    More, Less, Equal
};

class MoviesManager : public list<Movie> {
    /* Данный класс необходим для управления объектами типа Movie.
     * Класс является наследником std::list, поэтому хранит информацию в двусвязном списке.
     * Для добавления нового нода служит метод push_back, унаследованный от list.
     */
    static vector<int> prefixFunction(const string& str);
public:
    void loadFromDb(const string& db_path);
    void saveToDb(const string& db_path) const;
    bool removeMovie(const string& title, const string& release_date);
    bool editMovie(const string& title, const string& release_date, const Movie& movie);
    vector<const Movie*> search(const string& title = "", const string& genre = "", const string& release_date = "",
        double min_rating = -1, double max_rating = -1) const;
    vector<const Movie*> search(const string& cur_date, Compare compare) const;
    static bool containsWord(string word, const string text);

    // Следующие методы нужны, чтобы узнать, хранится ли уже данная ифнормация в объекте.
    bool duplicatePoster(const string& poster) const;
    bool duplicateTitleAndDate(const string& title, const string& date) const;
};
