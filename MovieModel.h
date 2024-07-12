#pragma once
#include <array>
#include <list>
#include <string>
#include <vector>

using namespace std;

class Movie {
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
    More, Less, Equal
};

class MoviesManager : public list<Movie> {
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
    bool duplicatePoster(const string& poster) const;
    bool duplicateTitleAndDate(const string& title, const string& date) const;
};
