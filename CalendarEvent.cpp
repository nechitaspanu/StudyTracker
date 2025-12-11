#include "CalendarEvent.h"

CalendarEvent::CalendarEvent(std::string label, const std::string &ymd)
    : label_(std::move(label)), date_(parseDate(ymd)) {
}

const std::string &CalendarEvent::label() const {
    return label_;
}

const Date& CalendarEvent::date() const {
    return date_;
}

std::ostream &operator<<(std::ostream &os, const CalendarEvent &e) {
    return os << "Event {" << e.label() << "->" << formatDate(e.date()) << "}";
}