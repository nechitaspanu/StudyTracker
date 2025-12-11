#ifndef OOP_UTILS_H
#define OOP_UTILS_H

#include <string>
#include <ctime>

using Date = std::string;
bool isValidYMD(const std::string& s);
Date parseDate(const std::string& ymd);
std::string formatDate(const Date& d);
std::time_t to_time_t_ymd(const Date& d);
Date todayDate();
int daysBetween(const Date& a, const Date& b);
std::string readLine(const std::string &prompt);
int readInt(const std::string &prompt, int lo, int hi);
std::string readDateYMD(const std::string &prompt);

#endif //OOP_UTILS_H