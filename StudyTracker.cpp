#include "StudyTracker.h"
#include "Utils.h"
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <unordered_set>
#include <fstream>
#include <cmath>

const Course *StudyTracker::findCourse(const std::string &name) const {
    for (const auto &c: courses_) if (c.name() == name) return &c;
    return nullptr;
}

std::string StudyTracker::keyCourse(const Course &c) {
    std::ostringstream k;
    k << c.name() << '|' << c.totalUnits() << '|' << c.completedUnits();
    return k.str();
}

std::string StudyTracker::keyAssignment(const Assignment &a) {
    std::ostringstream k;
    k << a.title() << '|' << a.notes() << '|' << formatDate(a.due());
    return k.str();
}

std::string StudyTracker::keyEvent(const CalendarEvent &e) {
    std::ostringstream k;
    k << e.label() << '|' << formatDate(e.date());
    return k.str();
}

void StudyTracker::addCourse(const Course &c) { courses_.push_back(c); }
void StudyTracker::addAssignment(const Assignment &a) { assignments_.push_back(a); }
void StudyTracker::addEvent(const CalendarEvent &e) { events_.push_back(e); }

bool StudyTracker::removeCourse(const std::string &name) {
    auto it = std::remove_if(courses_.begin(), courses_.end(),
                             [&](const Course &c) { return c.name() == name; });
    if (it == courses_.end()) return false;
    courses_.erase(it, courses_.end());
    return true;
}

bool StudyTracker::removeAssignment(const std::string &title) {
    auto it = std::remove_if(assignments_.begin(), assignments_.end(),
                             [&](const Assignment &a) { return a.title() == title; });
    if (it == assignments_.end()) return false;
    assignments_.erase(it, assignments_.end());
    return true;
}

bool StudyTracker::removeEvent(const std::string &label) {
    auto it = std::remove_if(events_.begin(), events_.end(),
                             [&](const CalendarEvent &e) { return e.label() == label; });
    if (it == events_.end()) return false;
    events_.erase(it, events_.end());
    return true;
}

bool StudyTracker::renameCourse(const std::string &oldName, const std::string &newName) {
    for (auto &c: courses_) {
        if (c.name() == oldName) {
            Course tmp(newName, c.totalUnits(), c.completedUnits());
            c = tmp;
            return true;
        }
    }
    return false;
}

bool StudyTracker::setCourseUnits(const std::string &name, int total, int done) {
    for (auto &c: courses_) {
        if (c.name() == name) {
            Course tmp(name, total, done);
            c = tmp;
            return true;
        }
    }
    return false;
}

bool StudyTracker::completeCourseUnits(const std::string &name, int units) {
    for (auto &c: courses_) {
        if (c.name() == name) {
            c.completeUnits(units);
            return true;
        }
    }
    return false;
}

bool StudyTracker::editAssignmentTitle(const std::string &oldTitle, const std::string &newTitle) {
    for (auto &a: assignments_) {
        if (a.title() == oldTitle) {
            Assignment tmp(newTitle, a.notes(), formatDate(a.due()));
            a = tmp;
            return true;
        }
    }
    return false;
}

bool StudyTracker::editAssignmentDue(const std::string &title, const std::string &newDueYMD) {
    for (auto &a: assignments_) {
        if (a.title() == title) {
            Assignment tmp(a.title(), a.notes(), newDueYMD);
            a = tmp;
            return true;
        }
    }
    return false;
}

bool StudyTracker::editAssignmentNotes(const std::string &title, const std::string &newNotes) {
    for (auto &a: assignments_) {
        if (a.title() == title) {
            Assignment tmp(a.title(), newNotes, formatDate(a.due()));
            a = tmp;
            return true;
        }
    }
    return false;
}

bool StudyTracker::renameEvent(const std::string &oldLabel, const std::string &newLabel) {
    for (auto &e: events_) {
        if (e.label() == oldLabel) {
            CalendarEvent tmp(newLabel, formatDate(e.date()));
            e = tmp;
            return true;
        }
    }
    return false;
}

bool StudyTracker::setEventDate(const std::string &label, const std::string &newDateYMD) {
    for (auto &e: events_) {
        if (e.label() == label) {
            CalendarEvent tmp(e.label(), newDateYMD);
            e = tmp;
            return true;
        }
    }
    return false;
}

bool StudyTracker::addTagToCourse(const std::string &name, const std::string &tg) {
    for (auto &c: courses_) if (c.name() == name) return c.addTag(tg);
    return false;
}

bool StudyTracker::removeTagFromCourse(const std::string &name, const std::string &tg) {
    for (auto &c: courses_) if (c.name() == name) return c.removeTag(tg);
    return false;
}

std::vector<Course> StudyTracker::filterByTag(const std::string &tg) const {
    std::vector<Course> out;
    for (const auto &c: courses_) if (c.hasTag(tg)) out.push_back(c);
    return out;
}

void StudyTracker::clearAll() {
    courses_.clear();
    assignments_.clear();
    events_.clear();
}

void StudyTracker::dedupe() {
    {
        std::unordered_set<std::string> seen;
        std::vector<Course> out;
        out.reserve(courses_.size());
        for (const auto &c: courses_) {
            auto k = keyCourse(c);
            if (seen.insert(k).second) out.push_back(c);
        }
        courses_.swap(out);
    }
    {
        std::unordered_set<std::string> seen;
        std::vector<Assignment> out;
        out.reserve(assignments_.size());
        for (const auto &a: assignments_) {
            auto k = keyAssignment(a);
            if (seen.insert(k).second) out.push_back(a);
        }
        assignments_.swap(out);
    }
    {
        std::unordered_set<std::string> seen;
        std::vector<CalendarEvent> out;
        out.reserve(events_.size());
        for (const auto &e: events_) {
            auto k = keyEvent(e);
            if (seen.insert(k).second) out.push_back(e);
        }
        events_.swap(out);
    }
}

double StudyTracker::overallProgress() const {
    long long total = 0, done = 0;
    for (const auto &c: courses_) {
        total += c.totalUnits();
        done += c.completedUnits();
    }
    if (total == 0) return 0.0;
    return 100.0 * static_cast<double>(done) / total;
}

std::vector<Assignment> StudyTracker::upcomingDeadlines(int days) const {
    std::vector<Assignment> result;
    for (const Assignment &a: assignments_) {
        if (a.isDueWithinDays(days)) {
            result.push_back(a);
        }
    }
    std::sort(result.begin(), result.end(),
              [](const Assignment &x, const Assignment &y) {
                  return x.due() < y.due();
              });
    return result;
}

std::string StudyTracker::courseReport(const std::string &name) const {
    const Course *c = findCourse(name);
    if (!c) return "Course not found";
    std::ostringstream os;
    os << *c;
    return os.str();
}

std::ostream &operator<<(std::ostream &os, const StudyTracker &st) {
    os << "---Courses---" << std::endl;
    for (const auto &c: st.courses_) os << "-" << c << "\n";
    os << "---Assignments---" << std::endl;
    for (const auto &a: st.assignments_) os << "-" << a << "\n";
    os << "---Events---" << std::endl;
    os << "---Goals---" << std::endl;
    if (st.goals_.empty()) {
        os << "(No goals yet)\n";
    } else {
        for (const auto* g : st.goals_) {
            os << "-" << *g;
            if (g->isAchieved()) os << " [COMPLET]";
            os << "\n";
        }
    }
    for (const auto &e: st.events_) os << "-" << e << "\n";
    os << "Overall progress:" << std::fixed << std::setprecision(1)
       << st.overallProgress() << "%\n";
    return os;
}

static std::string packNoSpaces(std::string s) {
    for (char &c: s) if (std::isspace(static_cast<unsigned char>(c))) c = '_';
    return s;
}

static std::string trim(const std::string &s) {
    size_t i = 0, j = s.size();
    while (i < j && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    while (j > i && std::isspace(static_cast<unsigned char>(s[j - 1]))) --j;
    return s.substr(i, j - i);
}

void saveToFile(const StudyTracker &st, const std::string &path) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        std::cerr << "ERROR saving to " << path << "\n";
        return;
    }

    for (const auto &c: st.courses_) {
        out << 1 << ' '
            << packNoSpaces(c.name()) << ' '
            << c.totalUnits() << ' '
            << c.completedUnits();

        if (!c.tags().empty()) {
            out << " |";
            for (size_t i = 0; i < c.tags().size(); ++i) {
                if (i) out << ',';
                out << c.tags()[i];
            }
        }
        out << '\n';
    }

    for (const auto &a: st.assignments_) {
        out << 2 << ' '
            << packNoSpaces(a.title()) << ' '
            << packNoSpaces(a.notes()) << ' '
            << formatDate(a.due()) << '\n';
    }

    for (const auto &e: st.events_) {
        out << 3 << ' '
            << packNoSpaces(e.label()) << ' '
            << formatDate(e.date()) << '\n';
    }

    for (const auto *g: st.goals_) {
        if (auto *rg = dynamic_cast<const ReadingGoal *>(g)) {
            out << 4 << ' '
                << 1 << ' '
                << packNoSpaces(g->getDescription()) << ' '
                << rg->getTarget() << ' '
                << rg->getRead() << '\n';
        } else if (auto *tg = dynamic_cast<const TimeGoal *>(g)) {
            out << 4 << ' '
                << 2 << ' '
                << packNoSpaces(g->getDescription()) << ' '
                << tg->getTarget() << ' '
                << tg->getDone() << '\n';
        } else if (auto *eg = dynamic_cast<const ExamGoal *>(g)) {
            out << 4 << ' '
                << 3 << ' '
                << packNoSpaces(g->getDescription()) << ' '
                << eg->getTarget() << ' '
                << eg->getCurrent() << '\n';
        }
    }
}

StudyTracker::~StudyTracker() {
    for (auto* g : goals_) {
        delete g;
    }
    goals_.clear();
}

void StudyTracker::addGoal(Goal* g) {
    goals_.push_back(g);
}

StudyTracker::StudyTracker(const StudyTracker& other)
    : courses_(other.courses_),
      assignments_(other.assignments_),
      events_(other.events_)
{

    for (const auto* g : other.goals_) {
        goals_.push_back(g->clone());
    }
}

StudyTracker& StudyTracker::operator=(const StudyTracker& other) {
    if (this == &other) return *this;

    StudyTracker temp(other);

    std::swap(courses_, temp.courses_);
    std::swap(assignments_, temp.assignments_);
    std::swap(events_, temp.events_);
    std::swap(goals_, temp.goals_);

    return *this;
}

void loadFromFiles(StudyTracker &st, const std::string &path) {
    std::ifstream fin(path);
    if (!fin) return;

    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream in(line);

        int tip;
        if (!(in >> tip)) continue;

        if (tip == 1) {
            auto firstSpace = line.find_first_of(" \t");
            if (firstSpace == std::string::npos) continue;
            std::string afterTip = trim(line.substr(firstSpace + 1));

            std::string tagsPart;
            auto barPos = afterTip.find('|');
            if (barPos != std::string::npos) {
                tagsPart = trim(afterTip.substr(barPos + 1));
                afterTip = trim(afterTip.substr(0, barPos));
            }

            std::istringstream in2(afterTip);
            std::string name;
            int total = 0, done = 0;
            if (!(in2 >> name >> total >> done)) continue;

            Course c(name, total, done);

            if (!tagsPart.empty()) {
                std::istringstream ts(tagsPart);
                std::string tg;
                while (std::getline(ts, tg, ',')) {
                    tg = trim(tg);
                    if (!tg.empty()) c.addTag(tg);
                }
            }
            st.addCourse(c);
        } else if (tip == 2) {
            std::string title, notes, due;
            if (in >> title >> notes >> due) {
                st.addAssignment(Assignment(title, notes, due));
            }
        } else if (tip == 3) {
            std::string label, date;
            if (in >> label >> date) {
                st.addEvent(CalendarEvent(label, date));
            }
        } else if (tip == 4) {
            int subType;
            std::string desc;
            if (in >> subType >> desc) {
                if (subType == 1) {
                    int target, read;
                    if (in >> target >> read) st.addGoal(new ReadingGoal(desc, target, read));
                } else if (subType == 2) {
                    int target, done;
                    if (in >> target >> done) st.addGoal(new TimeGoal(desc, target, done));
                } else if (subType == 3) {
                    double target, current;
                    if (in >> target >> current) st.addGoal(new ExamGoal(desc, target, current));
                }
            }
        }
    }
}

void StudyTracker::showBusiestDayStatistics() const {
    if (assignments_.empty()) {
        std::cout << "No assignments available to calculate statistics.\n";
        return;
    }

    // [STL MAP] Using a map to count date frequency
    std::map<std::string, int> frequencyMap;

    for (const auto& assign : assignments_) {
        // Assuming you have formatDate in Utils or Date::toString
        std::string dateStr = formatDate(assign.due());
        frequencyMap[dateStr]++;
    }

    // Finding the maximum
    std::string busiestDate;
    int maxCount = 0;

    for (const auto& pair : frequencyMap) {
        if (pair.second > maxCount) {
            maxCount = pair.second;
            busiestDate = pair.first;
        }
    }

    std::cout << "\n--- WORKLOAD STATISTICS ---\n";
    std::cout << "The busiest day is: " << busiestDate << "\n";
    std::cout << "Number of deadlines on that day: " << maxCount << "\n";

    if (maxCount >= 3) {
        std::cout << "WARNING: It will be a very busy day! Prepare in advance.\n";
    } else {
        std::cout << "Workload is manageable.\n";
    }
}
