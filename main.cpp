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
#include <fstream>
#include <filesystem>
#include <unordered_set>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

    static const std::filesystem::path& projectRoot() {
    static const std::filesystem::path p =
        std::filesystem::absolute(std::filesystem::path(__FILE__)).parent_path();
    return p;
    }

    const fs::path SEED_FILE = projectRoot() / "tastatura.txt";

    const fs::path SAVE_FILE = projectRoot() / "save_data.txt";



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
        delete[] data_;
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
        return os << tb.c_str();
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

            std::vector<std::string> tags_;

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

            const std::vector<std::string>& tags() const { return tags_; }

            bool hasTag(const std::string& tg) const {
            for (const auto& t : tags_) if (t == tg) return true;
            return false;
        }

            bool addTag(const std::string& tg) {
            if (tg.empty()) return false;
            if (hasTag(tg)) return false;
            tags_.push_back(tg);
            return true;
        }

            bool removeTag(const std::string& tg) {
            auto it = std::remove(tags_.begin(), tags_.end(), tg);
            if (it == tags_.end()) return false;
            tags_.erase(it, tags_.end());
            return true;
        }

    };

    std::ostream& operator<<(std::ostream& os, const Course& c) {
         os << "Course(name = '" << c.name() << ","
               << c.completedUnits() << "/" << c.totalUnits()
               << "(" << std::fixed << std::setprecision(1) << c.progress() << "%)}";

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

        static std::string keyCourse(const Course& c);
        static std::string keyAssignment(const Assignment& a);
        static std::string keyEvent(const CalendarEvent& e);


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

        void clearAll();
        void dedupe();

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
                          return x.due() < y.due();
                      });

            return result;
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

        friend void saveToFile(const StudyTracker& st, const std::string& path);

        bool completeCourseUnits(const std::string& name, int units) {
            for (auto& c : courses_) {
                if (c.name() == name) {
                    c.completeUnits(units);
                    return true;
                }
            }
            return false;
        }

        bool addTagToCourse(const std::string& name, const std::string& tg) {
            for (auto& c : courses_) if (c.name() == name) return c.addTag(tg);
            return false;
        }
        bool removeTagFromCourse(const std::string& name, const std::string& tg) {
            for (auto& c : courses_) if (c.name() == name) return c.removeTag(tg);
            return false;
        }
        std::vector<Course> filterByTag(const std::string& tg) const {
            std::vector<Course> out;
            for (const auto& c : courses_) if (c.hasTag(tg)) out.push_back(c);
            return out;
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

    void StudyTracker::clearAll() {
        courses_.clear();
        assignments_.clear();
        events_.clear();
    }

    std::string StudyTracker::keyCourse(const Course& c) {
        std::ostringstream k;
        k << c.name() << '|' << c.totalUnits() << '|' << c.completedUnits();
        return k.str();
    }

    std::string StudyTracker::keyAssignment(const Assignment& a) {
        std::ostringstream k;
        k << a.title() << '|' << a.notes().c_str() << '|' << formatDate(a.due());
        return k.str();
    }

    std::string StudyTracker::keyEvent(const CalendarEvent& e) {
        std::ostringstream k;
        k << e.label() << '|' << formatDate(e.date());
        return k.str();
    }

    void StudyTracker::dedupe() {
        {
            std::unordered_set<std::string> seen;
            std::vector<Course> out; out.reserve(courses_.size());
            for (const auto& c : courses_) {
                auto k = keyCourse(c);
                if (seen.insert(k).second) out.push_back(c);
            }
            courses_.swap(out);
        }

        {
            std::unordered_set<std::string> seen;
            std::vector<Assignment> out; out.reserve(assignments_.size());
            for (const auto& a : assignments_) {
                auto k = keyAssignment(a);
                if (seen.insert(k).second) out.push_back(a);
            }
            assignments_.swap(out);
        }

        {
            std::unordered_set<std::string> seen;
            std::vector<CalendarEvent> out; out.reserve(events_.size());
            for (const auto& e : events_) {
                auto k = keyEvent(e);
                if (seen.insert(k).second) out.push_back(e);
            }
            events_.swap(out);
        }
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


    void loadFromFiles(StudyTracker& st, const std::string& path);
    void saveToFiles(StudyTracker& st, const std::string& path);

    static std::string packNoSpaces(std::string s) {
    for (char& c : s) if (std::isspace(static_cast<unsigned char>(c))) c = '_';
    return s;
    }

    static std::string trim(std::string s) {
        size_t i = 0, j = s.size();
        while (i < j && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
        while (j > i && std::isspace(static_cast<unsigned char>(s[j-1]))) --j;
        return s.substr(i, j - i);
    }


    void loadFromFiles(StudyTracker& st, const std::string& path) {
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
                std::string name; int total = 0, done = 0;
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
            }
            else if (tip == 2) {
                std::string title, notes, due;
                if (in >> title >> notes >> due) {
                    st.addAssignment(Assignment(title, notes.c_str(), due));
                }
            }
            else if (tip == 3) {
                std::string label, date;
                if (in >> label >> date) {
                    st.addEvent(CalendarEvent(label, date));
                }
            }
        }
    }

void saveToFile(const StudyTracker& st, const std::string& path) {
        std::ofstream out(path, std::ios::trunc);
        if (!out) {
            std::cerr << "ERROR" << path << "\n";
            return;
        }

        for (const auto& c : st.courses_) {
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

        for (const auto& a : st.assignments_) {
            out << 2 << ' '
                << packNoSpaces(a.title()) << ' '
                << packNoSpaces(a.notes().c_str()) << ' '
                << formatDate(a.due()) << '\n';
        }

        for (const auto& e : st.events_) {
            out << 3 << ' '
                << packNoSpaces(e.label()) << ' '
                << formatDate(e.date()) << '\n';
        }
    }


    void AddTagToCourse(StudyTracker& st) {
        std::string name = readLine("Course title: ");
        std::string tg   = readLine("Tag (one word): ");
        std::cout << (st.addTagToCourse(name, tg) ? "Tag added.\n"
                                                  : "Course not found or tag already exists.\n");
    }

    void RemoveTagFromCourse(StudyTracker& st) {
        std::string name = readLine("Course title: ");
        std::string tg   = readLine("Tag to remove: ");
        std::cout << (st.removeTagFromCourse(name, tg) ? "Tag removed.\n"
                                                       : "Course not found or tag missing.\n");
    }

    void ListCoursesByTag(const StudyTracker& st) {
        std::string tg = readLine("Tag to filter by: ");
        auto v = st.filterByTag(tg);
        if (v.empty()) {
            std::cout << "No courses have tag '" << tg << "'.\n";
        } else {
            std::cout << "Courses with tag '" << tg << "':\n";
            for (const auto& c : v) std::cout << "  - " << c << "\n";
        }
    }



int main() {
    ios::sync_with_stdio(false);
    StudyTracker st;

    auto loadSeed = [&](){
        st.clearAll();
        loadFromFiles(st, SEED_FILE.string());
        st.dedupe();
        std::cout << "[seeded]  " << fs::absolute(SEED_FILE) << "\n";
    };
    auto loadSave = [&](){
        st.clearAll();
        loadFromFiles(st, SAVE_FILE.string());
        st.dedupe();
        std::cout << "[loaded]  " << fs::absolute(SAVE_FILE) << "\n";
    };
    auto save = [&](){
        st.dedupe();
        saveToFile(st, SAVE_FILE.string());
        std::cout << "[saved]   " << fs::absolute(SAVE_FILE) << "\n";
    };

    if (fs::exists(SAVE_FILE)) loadSave(); else loadSeed();

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
            << "18) Reset from tastatura.txt\n"
            << "19) Add tag to course\n"
            << "20) Remove tag from course\n"
            << "21) List courses by tag\n"
            << "0) Exit\n> ";

        int opt{};
        if (!(std::cin >> opt)) { save(); return 0; }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (opt) {
            case 0:  save(); return 0;

            case 1:  addCourseFromInput(st);       save(); break;
            case 2:  addAssignment(st);            save(); break;
            case 3:  addSessionDate(st);           save(); break;
            case 4:  std::cout << st << "\n";               break;

            case 5: {
                std::string name = readLine("Course title: ");
                std::cout << st.courseReport(name) << "\n";
                break;
            }

            case 6: {
                int days = readInt("Days ahead (>=1): ", 1, 3650);
                auto v = st.upcomingDeadlines(days);
                if (v.empty()) std::cout << "No assignments in the next " << days << " days.\n";
                else {
                    std::cout << "Assignments in the next " << days << " days:\n";
                    for (const auto& a : v) std::cout << "  * " << a << "\n";
                }
                break;
            }

            case 7: {
                std::string name = readLine("Course title: ");
                int units = readInt("Units to mark as completed (>=0): ", 0, 1'000'000);
                bool ok = st.completeCourseUnits(name, units);
                std::cout << (ok ? "Updated.\n" : "Course not found.\n");
                save(); break;
            }

            case 8:  RemoveCourse(st);             save(); break;
            case 9:  RemoveAssignment(st);         save(); break;
            case 10: RemoveEvent(st);              save(); break;
            case 11: RenameCourse(st);             save(); break;
            case 12: SetCourseUnits(st);           save(); break;
            case 13: EditAssignmentTitle(st);      save(); break;
            case 14: EditAssignmentDue(st);        save(); break;
            case 15: EditAssignmentNotes(st);      save(); break;
            case 16: RenameEvent(st);              save(); break;
            case 17: SetEventDate(st);             save(); break;

            case 18: {
                std::error_code ec;
                fs::remove(SAVE_FILE, ec);
                loadSeed();
                save();
                break;
            }

            case 19: AddTagToCourse(st); save(); break;
            case 20: RemoveTagFromCourse(st); save(); break;
            case 21: ListCoursesByTag(st);    break;

            default:
                std::cout << "Invalid option.\n";
                break;
        }
    }
}

