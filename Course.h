#ifndef OOP_COURSE_H
#define OOP_COURSE_H

#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

class Course {
private:
    std::string name_;
    int totalUnits_;
    int completedUnits_;
    std::vector<std::string> tags_;

    static int clamp(int v, int lo, int hi);

public:
    explicit Course(std::string name, int totalUnits, int completedUnits = 0);

    [[nodiscard]] const std::string &name() const;
    [[nodiscard]] int totalUnits() const;
    [[nodiscard]] int completedUnits() const;

    void completeUnits(int n);
    [[nodiscard]] double progress() const;

    const std::vector<std::string> &tags() const;
    bool hasTag(const std::string &tg) const;
    bool addTag(const std::string &tg);
    bool removeTag(const std::string &tg);
};

std::ostream &operator<<(std::ostream &os, const Course &c);

#endif //OOP_COURSE_H