#ifndef OOP_CALENDAREVENT_H
#define OOP_CALENDAREVENT_H

#pragma once
#include <string>
#include <iostream>
#include "Utils.h"

class CalendarEvent {
private:
    std::string label_;
    Date date_;

public:
    explicit CalendarEvent(std::string label, const std::string &ymd);

    [[nodiscard]] const std::string &label() const;
    [[nodiscard]] const Date& date() const;
};

std::ostream &operator<<(std::ostream &os, const CalendarEvent &e);

#endif //OOP_CALENDAREVENT_H