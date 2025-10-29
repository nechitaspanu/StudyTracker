#include <iostream>
#include <chrono>
#include <thread>
#include <ostream>
#include <SFML/Graphics.hpp>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>

using namespace std;

class TextBuffer {
private:
    char* data_;
    size_t len_;

    static char* duplicate(const char* s, size_t& outLen) {
        if (!s) { outLen = 0; return nullptr;}
        outLen = strlen(s);
        char* result = new char[outLen + 1];
        strcpy(result, s);
        return result;
    }

public:
    explicit TextBuffer(const char* s = nullptr) : data_(nullptr), len_(0) {
        data_ = duplicate(s, len_);
    }

    TextBuffer( const TextBuffer& other) : data_(nullptr), len_(0) {
        data_ = duplicate(other.data_, len_);
    }

    TextBuffer& operator = (const TextBuffer& other) {
        if (this == &other) return *this;
        delete[] data_; // elibereaza memoria curenta
        data_ = duplicate(other.data_, len_);
        return *this;

    }

    ~TextBuffer() { delete[] data_;}

    [[nodiscard]] const char* c_str() const {
        if (data_) return data_;
        return "";
    }

    [[nodiscard]] size_t size() const {return len_;}


    friend std::ostream& operator<<(std::ostream& os, const TextBuffer& tb) {
        return os << tb.c_str(); //nu stiu daca e bun
    }
    };
    static std::chrono::sys_days parseDate(const std::string& ymd) {
        int y, m, d;
        char c1, c2;
        istringstream is(ymd);
        if (!(is >> y >> c1 >> m >> c2 >> d) || c1 != '-' || c2 != '-') throw runtime_error ("Invalid date");
        using namespace std::chrono;

        year_month_day ymdv{ year{y},
                        month{static_cast<unsigned>(m)},
                        day{  static_cast<unsigned>(d)}
        };

        if (!ymdv.ok()) throw std::runtime_error("Invalid calendar date");
        return sys_days{ymdv};
}

    static std::string formatDate(std::chrono::sys_days dd) {
        using namespace std::chrono;
        year_month_day ymd = dd;
        ostringstream os;
        os << int(ymd.year()) <<
            '-'<< std::setw(2) << std::setfill('0') << static_cast<unsigned>(ymd.month()) <<
                '-'<< std::setw(2) << std::setfill('0') << static_cast<unsigned>(ymd.day());

        return os.str();
    }

std::string readLine(const std::string& prompt) {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);
        return s;
    }

int readInt(const std::string& prompt, int lo, int hi) {
        while (true) {
            std::cout << prompt;
            int x;
            if (std::cin >> x) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (x>=lo && x<=hi) return x;
            } else {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            std::cout << "Invalid input. Try again (" << lo << "..." << hi << ").\n";
        }


    }

std::string readDateYMD(const std::string& prompt) {
        while (true) {
            std::string s = readLine(prompt);
            try {
                (void) parseDate(s);
                return s;
            } catch (const std::exception& e) {
                std::cout<<"Invalid input: " << e.what()<< "(format: YYYY-MM-DD)\n";
            }
        }

    }

    class Assignment {
    private:
        string title_;
        TextBuffer notes_;
        chrono::sys_days due_;

    public:
        explicit Assignment(std::string title, const char* notes, const std::string& due)
            : title_(std::move(title)), notes_(notes), due_(parseDate(due)) {}

        [[nodiscard]]const string& title() const {return title_;}
        [[nodiscard]]const TextBuffer& notes() const {return notes_;}
        [[nodiscard]]std::chrono::sys_days due() const {return due_;}

        [[nodiscard]] bool isDueWithinDays(int daysCount) const{
            using namespace std::chrono;
            const sys_days today = floor <days> (system_clock::now());
            return due_ >= today && due_ <= today + days{daysCount};
        }
    };


        std::ostream& operator<<(std::ostream& os, const Assignment& a) {
            return os << "Assignment {title = '"  << a.title()
                  << "', due = " << formatDate(a.due())
                  << ", notes = '" << a.notes() << "'" << " (" << a.notes().size() << " chars)}";
    }

    class Course {
        private:
            string name_;
            int totalUnits_;
            int completedUnits_;
            static int clamp(int v, int lo, int hi) { return std::max(lo, std::min(v, hi));}

    public:
        explicit Course(string name, int totalUnits, int completedUnits = 0)
            : name_(std::move(name)),
              totalUnits_(max(0, totalUnits)),
              completedUnits_(clamp(completedUnits, 0, max(0, totalUnits))) {}

        [[nodiscard]]const std::string& name() const { return name_;}
        [[nodiscard]]int totalUnits() const { return totalUnits_;}
        [[nodiscard]]int completedUnits() const { return completedUnits_;}

        void completeUnits(int n) {
            completedUnits_ = clamp(completedUnits_ + max(0, n), 0, totalUnits_);
        }
            [[nodiscard]]double progress() const {
                if (totalUnits_ == 0) return 0.0;
                return 100.0 * static_cast<double>(completedUnits_) / static_cast<double>(totalUnits_);}
    };

    std::ostream& operator<<(std::ostream& os, const Course& c) {
         os << "Course(name = '" << c.name() << ","
               << c.completedUnits() << "/" << c.totalUnits()
               << "(" << std::fixed << std::setprecision(1) << c.progress() << "%)}";
        return os;
    }

class CalendarEvent {
    private:
        string label_;
        chrono::sys_days date_;

    public:
        explicit CalendarEvent(string label, const string& ymd) : label_(std::move(label)), date_(parseDate(ymd)) {}

        [[nodiscard]]const string& label() const { return label_;}
        [[nodiscard]]chrono::sys_days date() const { return date_;}
    };

    ostream& operator<<(ostream& os, const CalendarEvent& e) {
        return os << "Event {" << e.label() << "->" << formatDate(e.date()) << "}";
    }

    class StudyTracker {
    private:
        vector<Course> courses_;
        vector<Assignment> assignments_;
        vector<CalendarEvent> events_;

        [[nodiscard]]const Course* findCourse(const string& name) const {
            for (const auto&  c : courses_) if (c.name() == name ) return &c;
            return nullptr;
        }

    public:
        void addCourse(const Course& c) {courses_.push_back(c);}
        void addAssignment(const Assignment& a) {assignments_.push_back(a);}
        void addEvent(const CalendarEvent& e) {events_.push_back(e);}

        bool removeCourse(const std::string& name);
        bool removeAssignment(const std::string& title);
        bool removeEvent(const std::string& label);

        bool renameCourse(const string& oldName, const string& newName);
        bool setCourseUnits(const std::string& name, int total, int done);

        bool editAssignmentTitle(const std::string& oldTitle, const std::string& newTitle);
        bool editAssignmentDue(const std::string& title, const std::string& newDueYMD);
        bool editAssignmentNotes(const std::string& title, const std::string& newNotes);

        bool renameEvent(const std::string& oldLabel, const std::string& newLabel);
        bool setEventDate(const std::string& label, const std::string& newDateYMD);

        [[nodiscard]]double overallProgress() const {
            long long total = 0, done = 0;
            for (const auto& c : courses_ ) {
                total += c.totalUnits();
                done += c.completedUnits();}
            if (total == 0) {
                return 0.0;
            } else {
                return 100.0 * static_cast<double>(done) / total;
            }
        }

        [[nodiscard]]std::vector<Assignment> upcomingDeadlines(int days) const {
            std::vector<Assignment> result;

            for (const Assignment& a : assignments_) {
                if (a.isDueWithinDays(days)) {
                    result.push_back(a);
                }
            }

            std::sort(result.begin(), result.end(),
                      [](const Assignment& x, const Assignment& y) {
                          return x.due() < y.due();        // sortare după dată
                      });

            return result;                                    // returnăm vectorul
        }

        [[nodiscard]]string courseReport(const string& name) const {
            const Course* c = findCourse(name);
            if (!c) return "Course not found";
            ostringstream os;
            os << *c;
            return os.str();
        }

        friend ostream& operator<<(ostream& os, const StudyTracker& st) {
            os << "---Courses---" << endl;
            for (const auto& c : st.courses_) os << "-" << c << "\n";
            os << "---Assignments---" << endl;
            for (const auto& a : st.assignments_) os << "-" << a << "\n";
            os << "---Events---" << endl;
            for (const auto& e : st.events_) os << "-" << e << "\n";
            os << "Overall progress:" << fixed << setprecision(1)
            << st.overallProgress() << "%\n";
            return os;
        }

        bool completeCourseUnits(const std::string& name, int units) {
            for (auto& c : courses_) {
                if (c.name() == name) {
                    c.completeUnits(units);
                    return true;
                }
            }
            return false;
        }
    };

    bool StudyTracker::removeCourse(const std::string& name) {
    auto it = std::remove_if(courses_.begin(), courses_.end(),
                             [&](const Course& c){ return c.name() == name; });
    if (it == courses_.end()) return false;
    courses_.erase(it, courses_.end());
    return true;
    }

    bool StudyTracker::removeAssignment(const std::string& title) {
        auto it = std::remove_if(assignments_.begin(), assignments_.end(),
                                 [&](const Assignment& a){ return a.title() == title; });
        if (it == assignments_.end()) return false;
        assignments_.erase(it, assignments_.end());
        return true;
    }

    bool StudyTracker::removeEvent(const std::string& label) {
        auto it = std::remove_if(events_.begin(), events_.end(),
                                 [&](const CalendarEvent& e){ return e.label() == label; });
        if (it == events_.end()) return false;
        events_.erase(it, events_.end());
        return true;
    }

    bool StudyTracker::renameCourse(const std::string& oldName, const std::string& newName) {
        for (auto& c : courses_) {
            if (c.name() == oldName) {
                // mic "hack": refacem obiectul cu noul nume, păstrând unitățile
                Course tmp(newName, c.totalUnits(), c.completedUnits());
                c = tmp;
                return true;
            }
        }
        return false;
    }

    bool StudyTracker::setCourseUnits(const std::string& name, int total, int done) {
        for (auto& c : courses_) {
            if (c.name() == name) {
                Course tmp(name, std::max(0,total), std::max(0, std::min(done, std::max(0,total))));
                c = tmp;
                return true;
            }
        }
        return false;
    }

    bool StudyTracker::editAssignmentTitle(const std::string& oldTitle, const std::string& newTitle) {
        for (auto& a : assignments_) {
            if (a.title() == oldTitle) {
                Assignment tmp(newTitle, a.notes().c_str(), formatDate(a.due()));
                a = tmp;
                return true;
            }
        }
        return false;
    }

    bool StudyTracker::editAssignmentDue(const std::string& title, const std::string& newDueYMD) {
        for (auto& a : assignments_) {
            if (a.title() == title) {
                Assignment tmp(a.title(), a.notes().c_str(), newDueYMD);
                a = tmp;
                return true;
            }
        }
        return false;
    }

    bool StudyTracker::editAssignmentNotes(const std::string& title, const std::string& newNotes) {
        for (auto& a : assignments_) {
            if (a.title() == title) {
                Assignment tmp(a.title(), newNotes.c_str(), formatDate(a.due()));
                a = tmp;
                return true;
            }
        }
        return false;
    }

    bool StudyTracker::renameEvent(const std::string& oldLabel, const std::string& newLabel) {
        for (auto& e : events_) {
            if (e.label() == oldLabel) {
                CalendarEvent tmp(newLabel, formatDate(e.date()));
                e = tmp;
                return true;
            }
        }
        return false;
    }

    bool StudyTracker::setEventDate(const std::string& label, const std::string& newDateYMD) {
        for (auto& e : events_) {
            if (e.label() == label) {
                CalendarEvent tmp(e.label(), newDateYMD);
                e = tmp;
                return true;
            }
        }
        return false;
    }

    void addAssignment(StudyTracker& st) {
        std::string title = readLine("Homework: ");
        std::string notes = readLine("Notes: ");
        std::string due = readLine("Due (YYYY-MM-DD): ");

        st.addAssignment(Assignment(title, notes.c_str(), due));
        std::cout << "Added homework.\n";
    }

    void addCourseFromInput(StudyTracker& st) {
        std::string name = readLine("Course title: ");
        int total = readInt("Total courses (>=0):", 0, 10000);
        int done = readInt("Read courses (0 - total):", 0, total);

        st.addCourse(Course(name, total, done));
        std::cout << "Added course.\n";
    }

    void addSessionDate(StudyTracker& st) {
        while (true) {
            std::string label = readLine("Event title:");
            if (label.empty()) break;
            std::string date = readDateYMD("Date (YYYY-MM-DD): ");
            st.addEvent(CalendarEvent(label, date));
            std::cout << "Added event." << label << "-" << date << "\n";

        }
    }

    void RemoveCourse(StudyTracker& st) {
    std::string name = readLine("Course title to remove: ");
    std::cout << (st.removeCourse(name) ? "Removed.\n" : "Course not found.\n");
    }

    void RemoveAssignment(StudyTracker& st) {
    std::string t = readLine("Assignment title to remove: ");
    std::cout << (st.removeAssignment(t) ? "Removed.\n" : "Assignment not found.\n");
    }

    void RemoveEvent(StudyTracker& st) {
    std::string l = readLine("Event label to remove: ");
    std::cout << (st.removeEvent(l) ? "Removed.\n" : "Event not found.\n");
    }

    void RenameCourse(StudyTracker& st) {
    std::string oldN = readLine("Old course title: ");
    std::string newN = readLine("New course title: ");
    std::cout << (st.renameCourse(oldN, newN) ? "Renamed.\n" : "Course not found.\n");
    }

    void SetCourseUnits(StudyTracker& st) {
    std::string name = readLine("Course title: ");
    int total = readInt("New total units (>=0): ", 0, 1'000'000);
    int done  = readInt("New completed units (0..total): ", 0, total);
    std::cout << (st.setCourseUnits(name, total, done) ? "Updated.\n" : "Course not found.\n");
    }

    void EditAssignmentTitle(StudyTracker& st) {
    std::string oldT = readLine("Old assignment title: ");
    std::string newT = readLine("New assignment title: ");
    std::cout << (st.editAssignmentTitle(oldT, newT) ? "Updated.\n" : "Assignment not found.\n");
    }

    void EditAssignmentDue(StudyTracker& st) {
    std::string t = readLine("Assignment title: ");
    std::string d = readDateYMD("New due (YYYY-MM-DD): ");
    std::cout << (st.editAssignmentDue(t, d) ? "Updated.\n" : "Assignment not found.\n");
    }

    void EditAssignmentNotes(StudyTracker& st) {
    std::string t = readLine("Assignment title: ");
    std::string n = readLine("New notes: ");
    std::cout << (st.editAssignmentNotes(t, n) ? "Updated.\n" : "Assignment not found.\n");
    }

    void RenameEvent(StudyTracker& st) {
    std::string oldL = readLine("Old event label: ");
    std::string newL = readLine("New event label: ");
    std::cout << (st.renameEvent(oldL, newL) ? "Updated.\n" : "Event not found.\n");
    }

    void SetEventDate(StudyTracker& st) {
    std::string l = readLine("Event label: ");
    std::string d = readDateYMD("New date (YYYY-MM-DD): ");
    std::cout << (st.setEventDate(l, d) ? "Updated.\n" : "Event not found.\n");
    }


int main() {
        ios::sync_with_stdio(false);
        StudyTracker st;

        while (true) {
            std::cout
                << "\n1) Add course\n"
                << "2) Add homework\n"
                << "3) Add exam date\n"
                << "4) Show progress\n"
                << "5) Course report by name\n"
                << "6) Show deadlines in the next N days\n"
                << "7) Mark course units as completed\n"
                << "8) Remove course\n"
                << "9) Remove assignment\n"
                << "10) Remove event\n"
                << "11) Rename course\n"
                << "12) Set course units\n"
                << "13) Edit assignment title\n"
                << "14) Edit assignment due\n"
                << "15) Edit assignment notes\n"
                << "16) Rename event\n"
                << "17) Set event date\n"
                << "0) Exit\n> ";

            int opt{};
            if (!(std::cin >> opt)) {
                return 0;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            switch (opt) {
                case 0: return 0;
                case 1: addCourseFromInput(st); break;
                case 2: addAssignment(st); break;
                case 3: addSessionDate(st); break;
                case 4: std::cout << st << "\n"; break;
                case 5: {
                    std::string name = readLine("Course title: ");
                    std::cout << st.courseReport(name) << "\n";
                    break;
                }
                default:
                    std::cout << "Invalid option.\n";
                    break;
                case 6: {
                    int days = readInt("Days ahead (>=1): ", 1, 3650);
                    auto v = st.upcomingDeadlines(days);
                    if (v.empty()) {
                        std::cout << "No assignments in the next " << days << " days.\n";
                    } else {
                        std::cout << "Assignments in the next" << days << " days:\n";
                        for (const auto& a : v) std::cout << "  * " << a << "\n";
                    }
                    break;
                }
                case 7: {
                    std::string name = readLine("Course title: ");
                    int units = readInt("Units to mark as completed (>=0): ", 0, 1'000'000);

                    bool ok = st.completeCourseUnits(name, units);
                    std::cout << (ok ? "Updated.\n" : "Course not found.\n");
                    break;
                }
                case 8:  RemoveCourse(st); break;
                case 9:  RemoveAssignment(st); break;
                case 10: RemoveEvent(st); break;
                case 11: RenameCourse(st); break;
                case 12: SetCourseUnits(st); break;
                case 13: EditAssignmentTitle(st); break;
                case 14: EditAssignmentDue(st); break;
                case 15: EditAssignmentNotes(st); break;
                case 16: RenameEvent(st); break;
                case 17: SetEventDate(st); break;

            }
        }


        return 0;
    }
