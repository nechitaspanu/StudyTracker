#include "Assignment.h"
#include "Utils.h"

int Assignment::urgencyThresholdDays = 3;

Assignment::Assignment(std::string title, std::string notes, const std::string &due)
    : title_(std::move(title)), notes_(std::move(notes)), due_(parseDate(due)) {
}

const std::string &Assignment::title() const {
    return title_;
}

const std::string &Assignment::notes() const {
    return notes_;
}

const Date& Assignment::due() const {
    return due_;
}

bool Assignment::isUrgent() const {
    return isDueWithinDays(urgencyThresholdDays);
}

bool Assignment::isDueWithinDays(int daysCount) const {
    Date today = todayDate();
    int diff = daysBetween(today, due_);
    return diff >= 0 && diff <= daysCount;
}

void Assignment::print(std::ostream& os) const {
    os << "Assignment {title = '" << title_
       << "', due = " << formatDate(due_)
       << ", notes = '" << notes_ << "'"
       << " (" << notes_.size() << " chars)}";

    if (isUrgent()) {
        os << " [URGENT!]";
    }
}

std::ostream &operator<<(std::ostream &os, const Assignment &a) {
    a.print(os);
    return os;
}