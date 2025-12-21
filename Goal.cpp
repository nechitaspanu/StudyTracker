#include "Goal.h"

Goal::Goal(std::string desc) : description_(std::move(desc)) {}

std::ostream& operator<<(std::ostream& os, const Goal& g) {
    g.print(os);
    return os;
}

ReadingGoal::ReadingGoal(std::string desc, int target, int read)
    : Goal(std::move(desc)), pagesTarget_(target), pagesRead_(read) {}

bool ReadingGoal::isAchieved() const {
    return pagesRead_ >= pagesTarget_;
}

void ReadingGoal::print(std::ostream& os) const {
    os << "[Book] " << description_ << " (" << pagesRead_ << "/" << pagesTarget_ << " page)";
}

TimeGoal::TimeGoal(std::string desc, int target, int done)
    : Goal(std::move(desc)), minutesTarget_(target), minutesDone_(done) {}

bool TimeGoal::isAchieved() const {
    return minutesDone_ >= minutesTarget_;
}

void TimeGoal::print(std::ostream& os) const {
    os << "[Time] " << description_ << " (" << minutesDone_ << "/" << minutesTarget_ << " min)";
}

ExamGoal::ExamGoal(std::string desc, double target, double current)
    : Goal(std::move(desc)), gradeTarget_(target), currentGrade_(current) {}

bool ExamGoal::isAchieved() const {
    return currentGrade_ >= gradeTarget_;
}

void ExamGoal::print(std::ostream& os) const {
    os << "[Grade] " << description_ << " (Current: " << currentGrade_ << " / Goal: " << gradeTarget_ << ")";
}