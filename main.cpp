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
        istringstream is(ymd); //permite sa parcurgem ca intr un stream (ca cin)
        if (!(is >> y >> c1 >> m >> c2 >> d) || c1 != '-' || c2 != '-') throw runtime_error ("invalid date");
        using namespace std::chrono;

        year_month_day ymdv{ year{y},
                        month{static_cast<unsigned>(m)},
                        day{  static_cast<unsigned>(d)}
        };

        if (!ymdv.ok()) throw std::runtime_error("invalid calendar date");
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
        return os << "Event {" << e.label() << "@" << formatDate(e.date()) << "}";
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
    };

int main() {

    ios::sync_with_stdio(false);
    StudyTracker st;

    //cursuri
    Course poo{"POO", 12, 7};
    poo.completeUnits(2);
    st.addCourse(poo);
    st.addCourse(Course("Algoritmi", 10, 4));
    st.addCourse(Course("Baze de date", 8, 2));

    //teme
    st.addAssignment(Assignment("Tema 1 POO", "clase", "2025-11-10"));
    st.addAssignment(Assignment("Tema 2 Algoritmi", "Divide et Impera", "2025-12-15"));
    st.addAssignment(Assignment("BD Lab 3", "join", "2026-01-12"));

    st.addEvent(CalendarEvent("Sesiune iarna", "2026-01-20"));
    st.addEvent(CalendarEvent("Colocviu POO", "2025-12-20"));

    cout << st << "\n";
    cout << "Raport POO" << st.courseReport("POO") << "\n";
    auto soon = st.upcomingDeadlines(14);
    cout << "\nDeadline in urmatoarele 14 zile:\n";
    for (const auto& a : soon) cout << "* " << a << "\n";

    return 0;
}
