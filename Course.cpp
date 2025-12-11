#include "Course.h"
#include <iostream>
#include <iomanip>

int Course::clamp(int v, int lo, int hi) {
    return std::max(lo, std::min(v, hi));
}

Course::Course(std::string name, int totalUnits, int completedUnits)
    : name_(std::move(name)),
      totalUnits_(std::max(0, totalUnits)),
      completedUnits_(clamp(completedUnits, 0, std::max(0, totalUnits))) {
}

const std::string &Course::name() const {
    return name_;
}

int Course::totalUnits() const {
    return totalUnits_;
}

int Course::completedUnits() const {
    return completedUnits_;
}

void Course::completeUnits(int n) {
    completedUnits_ = clamp(completedUnits_ + std::max(0, n), 0, totalUnits_);
}

double Course::progress() const {
    if (totalUnits_ == 0) return 0.0;
    return 100.0 * static_cast<double>(completedUnits_) / static_cast<double>(totalUnits_);
}

const std::vector<std::string> &Course::tags() const {
    return tags_;
}

bool Course::hasTag(const std::string &tg) const {
    for (const auto &t: tags_) {
        if (t == tg) return true;
    }
    return false;
}

bool Course::addTag(const std::string &tg) {
    if (tg.empty()) return false;
    if (hasTag(tg)) return false;
    tags_.push_back(tg);
    return true;
}

bool Course::removeTag(const std::string &tg) {
    auto it = std::remove(tags_.begin(), tags_.end(), tg);
    if (it == tags_.end()) return false;
    tags_.erase(it, tags_.end());
    return true;
}

std::ostream &operator<<(std::ostream &os, const Course &c) {
    os << "Course(name = '" << c.name() << "', "
       << c.completedUnits() << "/" << c.totalUnits()
       << " (" << std::fixed << std::setprecision(1) << c.progress() << "%))";

    os << " [tags:";
    if (c.tags().empty()) os << "-";
    else {
        for (size_t i = 0; i < c.tags().size(); ++i) {
            if (i) os << ',';
            os << c.tags()[i];
        }
    }
    os << "]";
    return os;
}