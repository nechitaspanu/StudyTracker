
#ifndef OOP_ASSIGNMENT_H
#define OOP_ASSIGNMENT_H

#pragma once
#include <string>
#include <iostream>
#include "Utils.h"

class Assignment {
private:
    std::string title_;
    std::string notes_;
    Date due_;

public:
    explicit Assignment(std::string title, std::string notes, const std::string &due);

    const std::string &title() const;
    const std::string &notes() const;
    const Date& due() const;

    bool isDueWithinDays(int daysCount) const;
};

std::ostream &operator<<(std::ostream &os, const Assignment &a);

#endif //OOP_ASSIGNMENT_H