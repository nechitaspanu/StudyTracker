
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

    static int urgencyThresholdDays;

public:
    Assignment(std::string title, std::string notes, const Date &due);

    void print(std::ostream& os) const;

    const std::string &title() const;
    const std::string &notes() const;
    const Date& due() const;

    static void setUrgencyThreshold(int days) {
        urgencyThresholdDays = days;
    }
    static int getUrgencyThreshold() {
        return urgencyThresholdDays;
    }

    bool isUrgent() const;
    bool isDueWithinDays(int daysCount) const;
};

std::ostream &operator<<(std::ostream &os, const Assignment &a);

#endif //OOP_ASSIGNMENT_H