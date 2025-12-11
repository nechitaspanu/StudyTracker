#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <limits>

bool isValidYMD(const std::string& s) {
    if (s.size() != 10 || s[4] != '-' || s[7] != '-') return false;
    auto dig = [](char c){ return c >= '0' && c <= '9'; };
    for (int i : {0,1,2,3,5,6,8,9}) if (!dig(s[i])) return false;

    int y = std::stoi(s.substr(0,4));
    int m = std::stoi(s.substr(5,2));
    int d = std::stoi(s.substr(8,2));

    if (m < 1 || m > 12) return false;
    static const int md[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    int mdays = md[m];
    if (m == 2 && ((y%4==0 && y%100!=0) || (y%400==0))) mdays = 29;
    return d >= 1 && d <= mdays;
}

Date parseDate(const std::string& ymd) {
    if (!isValidYMD(ymd)) throw std::runtime_error("Invalid date");
    return ymd;
}
std::string formatDate(const Date& d) {
    return d;
}

std::time_t to_time_t_ymd(const Date& d) {
    int y = std::stoi(d.substr(0,4));
    int m = std::stoi(d.substr(5,2));
    int dd= std::stoi(d.substr(8,2));
    std::tm tm{};
    tm.tm_year = y - 1900;
    tm.tm_mon  = m - 1;
    tm.tm_mday = dd;
    tm.tm_hour = 0; tm.tm_min = 0; tm.tm_sec = 0;
    tm.tm_isdst = -1;
    return std::mktime(&tm);
}

Date todayDate() {
    std::time_t t = std::time(nullptr);
    const std::tm* lt = std::localtime(&t);
    std::ostringstream os;
    os << (1900 + lt->tm_year) << '-'
       << std::setw(2) << std::setfill('0') << (1 + lt->tm_mon) << '-'
       << std::setw(2) << std::setfill('0') << lt->tm_mday;
    return os.str();
}

int daysBetween(const Date& a, const Date& b) {
    long long sec = std::llround(std::difftime(to_time_t_ymd(b), to_time_t_ymd(a)));
    return static_cast<int>(sec / 86400);
}

std::string readLine(const std::string &prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s); // daca pun std::cin >> std::ws nu mai merge sa opresc addSessionDate
    return s;
}

int readInt(const std::string &prompt, int lo, int hi) {
    while (true) {
        std::cout << prompt;
        int x;
        if (std::cin >> x) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (x >= lo && x <= hi) return x;
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cout << "Invalid input. Try again (" << lo << "..." << hi << ").\n";
    }
}

std::string readDateYMD(const std::string &prompt) {
    while (true) {
        std::string s = readLine(prompt);
        try {
            (void) parseDate(s);
            return s;
        } catch (const std::exception &e) {
            std::cout << "Invalid input: " << e.what() << "(format: YYYY-MM-DD)\n";
        }
    }
}

