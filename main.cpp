#include <iostream>
#include <chrono>
#include <thread>
#include <ostream>
#include <SFML/Graphics.hpp>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>
#include <chrono>
#include <iomanip>
#include <array>
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

    const char* c_str() const {
        if (data_) return data_;
        return "";
    }

    size_t size() const {return len_;}


    friend std::ostream& operator<<(std::ostream& os, const TextBuffer& tb) {
        return os << tb.c_str(); //nu stiu daca e bun
    }
    };
    static chrono::sys_days parseDate(const std::string& ymd) {
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
        os << int(ymd.year()) << '-'
        << std::setw(2) << std::setfill('0') << static_cast<unsigned>(ymd.month()) << '-'
        << std::setw(2) << std::setfill('0') << static_cast<unsigned>(ymd.day());

        return os.str();
    }

    class Assignment {
    private:
        string title_;
        TextBuffer notes_;
        chrono::sys_days due_;

    public:
        explicit Assignment(string title, const char* notes, const string& due)
            : title_(move(title)), notes_(notes), due_(parseDate(due)) {}

        const string& title() const {return title_;}
        const TextBuffer& notes() const {return notes_;}
        chrono::sys_days due() const {return due_;}

        bool isDueWithinDays(int daysCount) const{
            using namespace std::chrono;
            const sys_days today = floor <days> (system_clock::now());
            return due_ >= today && due_ <= today + days{daysCount};
        }
    };

    ostream& operator<<(ostream& os, const Assignment& a) {
        return os << "Assignment {title = '"  << a.title()
                  << ", due = " << formatDate(a.due())
                  << ", notes = " << a.notes() << "'}";
    }

    class Course {
        private:
            string name_;
            int totalUnits_;
            int completedUnits_;
            int clamp(int v, int lo, int hi) { return max(lo, min(v, hi));}

    public:
        explicit Course(string name, int totalUnits, int completedUnits = 0)
            : name_(move(name)),
              totalUnits_(max(0, totalUnits)),
              completedUnits_(clamp(completedUnits, 0, max(0, totalUnits))) {}

        const string& name() const { return name_;}
        int totalUnits() const { return totalUnits_;}
        int completedUnits() const { return completedUnits_;}

        void completeUnits(int n) {
            completedUnits_ = clamp(completedUnits_ + max(0, n), 0, totalUnits_);
        }
            double progress() const {
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
        explicit CalendarEvent(string label, const string& ymd) : label_(move(label)), date_(parseDate(ymd)) {}

        const string& label() const { return label_;}
        chrono::sys_days date() const { return date_;}
    };

    ostream& operator<<(ostream& os, const CalendarEvent& e) {
        return os << "Event {" << e.label() << "@" << formatDate(e.date()) << "}";
    }

    class StudyTracker {
    private:
        vector<Course> courses_;
        vector<Assignment> assignments_;
        vector<CalendarEvent> events_;

        const Course* findCourse(const string& name) const {
            for (const auto&  c : courses_) if (c.name() == name ) return &c;
            return nullptr;
        }

    public:
        void addCourse(const Course& c) {courses_.push_back(c);}
        void addAssignment(const Assignment& a) {assignments_.push_back(a);}
        void addEvent(const CalendarEvent& e) {events_.push_back(e);}

        double overallProgress() const {
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

        std::vector<Assignment> upcomingDeadlines(int days) const {
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

        string courseReport(const string& name) const {
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

    /////////////////////////////////////////////////////////////////////////
    /// Observație: dacă aveți nevoie să citiți date de intrare de la tastatură,
    /// dați exemple de date de intrare folosind fișierul tastatura.txt
    /// Trebuie să aveți în fișierul tastatura.txt suficiente date de intrare
    /// (în formatul impus de voi) astfel încât execuția programului să se încheie.
    /// De asemenea, trebuie să adăugați în acest fișier date de intrare
    /// pentru cât mai multe ramuri de execuție.
    /// Dorim să facem acest lucru pentru a automatiza testarea codului, fără să
    /// mai pierdem timp de fiecare dată să introducem de la zero aceleași date de intrare.
    ///
    /// Pe GitHub Actions (bife), fișierul tastatura.txt este folosit
    /// pentru a simula date introduse de la tastatură.
    /// Bifele verifică dacă programul are erori de compilare, erori de memorie și memory leaks.
    ///
    /// Dacă nu puneți în tastatura.txt suficiente date de intrare, îmi rezerv dreptul să vă
    /// testez codul cu ce date de intrare am chef și să nu pun notă dacă găsesc vreun bug.
    /// Impun această cerință ca să învățați să faceți un demo și să arătați părțile din
    /// program care merg (și să le evitați pe cele care nu merg).
    ///
    /////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    /// Pentru date citite din fișier, NU folosiți tastatura.txt. Creați-vă voi
    /// alt fișier propriu cu ce alt nume doriți.
    /// Exemplu:
    /// std::ifstream fis("date.txt");
    /// for(int i = 0; i < nr2; ++i)
    ///     fis >> v2[i];
    ///
    ///////////////////////////////////////////////////////////////////////////
    ios::sync_with_stdio(false);
    StudyTracker st;

    //cursuri
    st.addCourse(Course("POO", 12, 3));
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



   // sf::RenderWindow window;
    ///////////////////////////////////////////////////////////////////////////
    /// NOTE: sync with env variable APP_WINDOW from .github/workflows/cmake.yml:31
    // window.create(sf::VideoMode({800, 700}), "My Window", sf::Style::Default);
    ///////////////////////////////////////////////////////////////////////////
    //std::cout << "Fereastra a fost creată\n";
    ///////////////////////////////////////////////////////////////////////////
    /// NOTE: mandatory use one of vsync or FPS limit (not both)            ///
    /// This is needed so we do not burn the GPU                            ///
   // window.setVerticalSyncEnabled(true);                                    ///
    /// window.setFramerateLimit(60);                                       ///
    ///////////////////////////////////////////////////////////////////////////

    //while(window.isOpen()) {
        //bool shouldExit = false;

       // while(const std::optional event = window.pollEvent()) {
            //if (event->is<sf::Event::Closed>()) {
                //window.close();
                //std::cout << "Fereastra a fost închisă\n";
           // }
           // else if (event->is<sf::Event::Resized>()) {
                //std::cout << "New width: " << window.getSize().x << '\n'
                         // << "New height: " << window.getSize().y << '\n';
           // }
           // else if (event->is<sf::Event::KeyPressed>()) {
              //  const auto* keyPressed = event->getIf<sf::Event::KeyPressed>();
             //   std::cout << "Received key " << (keyPressed->scancode == sf::Keyboard::Scancode::X ? "X" : "(other)") << "\n";
               // if(keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                  //  shouldExit = true;
            //    }
          //  }
    //    }
      //  if(shouldExit) {
          //  window.close();
           // std::cout << "Fereastra a fost închisă (shouldExit == true)\n";
            //break;
       // }
       // using namespace std::chrono_literals;
      //  std::this_thread::sleep_for(300ms);

       // window.clear();
     //   window.display();
   // }

    //std::cout << "Programul a terminat execuția\n";
    return 0;
}
