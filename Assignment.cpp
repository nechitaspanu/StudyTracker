#include "Assignment.h"
#include "Utils.h"

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

bool Assignment::isDueWithinDays(int daysCount) const {
    Date today = todayDate();
    int diff = daysBetween(today, due_);
    return diff >= 0 && diff <= daysCount;
}

std::ostream &operator<<(std::ostream &os, const Assignment &a) {
    return os << "Assignment {title = '" << a.title()
           << "', due = " << formatDate(a.due())
           << ", notes = '" << a.notes() << "'" << " (" << a.notes().size() << " chars)}";
}