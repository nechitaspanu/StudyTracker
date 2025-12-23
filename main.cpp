#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <iomanip>
#include <ostream>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <unordered_set>
#include <filesystem>
#include <cctype>

#include "Utils.h"
#include "Assignment.h"
#include "Course.h"
#include "CalendarEvent.h"
#include "StudyTracker.h"
#include "Goal.h"
#include "Exceptions.h"

using namespace std;

namespace fs = std::filesystem;

static fs::path computeSaveFilePath(const char *argv0) {
    fs::path base;
    if (argv0 && *argv0) {
        fs::path candidate(argv0);
        if (candidate.is_relative()) {
            candidate = fs::current_path() / candidate;
        }
        base = candidate.parent_path();
    }
    if (base.empty()) {
        base = fs::current_path();
    }
    return fs::absolute(base / "save_data.txt");
}

void addAssignment(StudyTracker &st) {
    std::string title = readLine("Homework: ");
    std::string notes = readLine("Notes: ");
    std::string due = readLine("Due (YYYY-MM-DD): ");

    st.addAssignment(Assignment(title, notes, due));
    std::cout << "Added homework.\n";
}

void addCourseFromInput(StudyTracker &st) {
    std::string name = readLine("Course title: ");
    int total = readInt("Total courses (>=0):", 0, 10000);
    int done = readInt("Read courses (0 - total):", 0, total);

    st.addCourse(Course(name, total, done));
    std::cout << "Added course.\n";
}

void addSessionDate(StudyTracker &st) {
    while (true) {
        std::string label = readLine("Event title:");
        if (label.empty()) break;
        std::string date = readDateYMD("Date (YYYY-MM-DD): ");
        st.addEvent(CalendarEvent(label, date));
        std::cout << "Added event." << label << "-" << date << "\n";
    }
}

void RemoveCourse(StudyTracker &st) {
    std::string name = readLine("Course title to remove: ");
    std::cout << (st.removeCourse(name) ? "Removed.\n" : "Course not found.\n");
}

void RemoveAssignment(StudyTracker &st) {
    std::string t = readLine("Assignment title to remove: ");
    std::cout << (st.removeAssignment(t) ? "Removed.\n" : "Assignment not found.\n");
}

void RemoveEvent(StudyTracker &st) {
    std::string l = readLine("Event label to remove: ");
    std::cout << (st.removeEvent(l) ? "Removed.\n" : "Event not found.\n");
}

void RenameCourse(StudyTracker &st) {
    std::string oldN = readLine("Old course title: ");
    std::string newN = readLine("New course title: ");
    std::cout << (st.renameCourse(oldN, newN) ? "Renamed.\n" : "Course not found.\n");
}

void SetCourseUnits(StudyTracker &st) {
    std::string name = readLine("Course title: ");
    int total = readInt("New total units (>=0): ", 0, 1'000'000);
    int done = readInt("New completed units (0..total): ", 0, total);
    std::cout << (st.setCourseUnits(name, total, done) ? "Updated.\n" : "Course not found.\n");
}

void EditAssignmentTitle(StudyTracker &st) {
    std::string oldT = readLine("Old assignment title: ");
    std::string newT = readLine("New assignment title: ");
    std::cout << (st.editAssignmentTitle(oldT, newT) ? "Updated.\n" : "Assignment not found.\n");
}

void EditAssignmentDue(StudyTracker &st) {
    std::string t = readLine("Assignment title: ");
    std::string d = readDateYMD("New due (YYYY-MM-DD): ");
    std::cout << (st.editAssignmentDue(t, d) ? "Updated.\n" : "Assignment not found.\n");
}

void EditAssignmentNotes(StudyTracker &st) {
    std::string t = readLine("Assignment title: ");
    std::string n = readLine("New notes: ");
    std::cout << (st.editAssignmentNotes(t, n) ? "Updated.\n" : "Assignment not found.\n");
}

void RenameEvent(StudyTracker &st) {
    std::string oldL = readLine("Old event label: ");
    std::string newL = readLine("New event label: ");
    std::cout << (st.renameEvent(oldL, newL) ? "Updated.\n" : "Event not found.\n");
}

void SetEventDate(StudyTracker &st) {
    std::string l = readLine("Event label: ");
    std::string d = readDateYMD("New date (YYYY-MM-DD): ");
    std::cout << (st.setEventDate(l, d) ? "Updated.\n" : "Event not found.\n");
}

void AddTagToCourse(StudyTracker &st) {
    std::string name = readLine("Course title: ");
    std::string tg = readLine("Tag (one word): ");
    std::cout << (st.addTagToCourse(name, tg)
                      ? "Tag added.\n"
                      : "Course not found or tag already exists.\n");
}

void RemoveTagFromCourse(StudyTracker &st) {
    std::string name = readLine("Course title: ");
    std::string tg = readLine("Tag to remove: ");
    std::cout << (st.removeTagFromCourse(name, tg)
                      ? "Tag removed.\n"
                      : "Course not found or tag missing.\n");
}

void ListCoursesByTag(const StudyTracker &st) {
    std::string tg = readLine("Tag to filter by: ");
    auto v = st.filterByTag(tg);
    if (v.empty()) {
        std::cout << "No courses have tag '" << tg << "'.\n";
    } else {
        std::cout << "Courses with tag '" << tg << "':\n";
        for (const auto &c: v) std::cout << "  - " << c << "\n";
    }
}

void addGoalMenu(StudyTracker &st) {
    std::cout << "Goal: 1) Reading, 2) Time, 3) Grade: ";
    int type = readInt("", 1, 3);
    std::string desc = readLine("Description: ");

    if (type == 1) {
        int target = readInt("Page goal: ", 1, 1000);
        int done = readInt("Read pages: ", 0, target);
        st.addGoal(new ReadingGoal(desc, target, done));
    }
    else if (type == 2) {
        int target = readInt("Target time (mins): ", 1, 600);
        int done = readInt("Completed time (mins): ", 0, target);
        st.addGoal(new TimeGoal(desc, target, done));
    }
    else {
        int target = readInt("Target grade (int): ", 1, 10);
        int current = readInt("Obtained grade (int): ", 1, 10);
        st.addGoal(new ExamGoal(desc, (double)target, (double)current));
    }
    std::cout << "Objective added!\n";
}


int main(int argc, char **argv) {
    ios::sync_with_stdio(false);

    StudyTracker st;

    const fs::path SAVE_FILE = computeSaveFilePath(argc > 0 ? argv[0] : nullptr);

    std::error_code ec;
    std::filesystem::create_directories(SAVE_FILE.parent_path(), ec);

    if (!std::filesystem::exists(SAVE_FILE)) {
        std::ofstream(SAVE_FILE).close();
        std::cout << "[created] " << std::filesystem::absolute(SAVE_FILE) << "\n";
    }

    if (std::filesystem::exists(SAVE_FILE)) {
        st.clearAll();
        loadFromFiles(st, SAVE_FILE.string());
        st.dedupe();
        std::cout << "[loaded] " << std::filesystem::absolute(SAVE_FILE) << "\n";
    } else {
        std::cout << "[empty database]\n";
    }

    auto save = [&](){
        st.dedupe();
        std::ofstream out(SAVE_FILE, std::ios::trunc);
        if (!out) {
            std::cerr << "[save ERROR] cannot open " << SAVE_FILE << "\n";
            return;
        }
        saveToFile(st, SAVE_FILE.string());
        std::cout << "[saved]   " << std::filesystem::absolute(SAVE_FILE) << "\n";
    };

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
                << "18) Add tag to course\n"
                << "19) Remove tag from course\n"
                << "20) List courses by tag\n"
                << "21) Add new goal\n"
                << "22) Set assignment urgency threshold\n"
                << "23) Show busiest day statistics\n"
                << "24) Log study session\n"
                << "25) Show study history\n"
                << "0) Exit\n> ";

        int opt{};
        if (!(std::cin >> opt)) {
            save();
            return 0;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (opt) {
            case 0: save();
                return 0;
            case 1: addCourseFromInput(st);
                save();
                break;
            case 2: addAssignment(st);
                save();
                break;
            case 3: addSessionDate(st);
                save();
                break;
            case 4: std::cout << st << "\n";
                break;
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
                    for (const auto &a: v) std::cout << "  * " << a << "\n";
                }
                break;
            }
            case 7: {
                std::string name = readLine("Course title: ");
                int units = readInt("Units to mark as completed (>=0): ", 0, 1'000'000);
                std::cout << (st.completeCourseUnits(name, units) ? "Updated.\n" : "Course not found.\n");
                save();
                break;
            }
            case 8: RemoveCourse(st);
                save();
                break;
            case 9: RemoveAssignment(st);
                save();
                break;
            case 10: RemoveEvent(st);
                save();
                break;
            case 11: RenameCourse(st);
                save();
                break;
            case 12: SetCourseUnits(st);
                save();
                break;
            case 13: EditAssignmentTitle(st);
                save();
                break;
            case 14: EditAssignmentDue(st);
                save();
                break;
            case 15: EditAssignmentNotes(st);
                save();
                break;
            case 16: RenameEvent(st);
                save();
                break;
            case 17: SetEventDate(st);
                save();
                break;
            case 18: AddTagToCourse(st);
                save();
                break;
            case 19: RemoveTagFromCourse(st);
                save();
                break;
            case 20: ListCoursesByTag(st);
                break;
            case 21: addGoalMenu(st);
                break;
            case 22: {
                std::cout << "\nCurrent urgency threshold: "
                          << Assignment::getUrgencyThreshold() << " days.\n";
                try {
                    int newDays;
                    std::cout << "Enter new urgency threshold (days): ";
                    if (!(std::cin >> newDays)) {
                        std::cin.clear();
                        std::cin.ignore(1000, '\n');
                        throw InvalidInputException("Input is not a valid integer!");
                    }
                    if (newDays < 0) {
                        throw InvalidInputException("Days count cannot be negative!");
                    }

                    Assignment::setUrgencyThreshold(newDays);
                    std::cout << "Success! Global rule updated.\n";
                }
                catch (const InvalidInputException& e) {
                    std::cerr << "\n[!!! EXCEPTION CAUGHT !!!] " << e.what() << "\n";
                    std::cerr << "Action cancelled. Please try again with a positive number.\n";
                }
                catch (const std::exception& e) {
                    std::cerr << "Unexpected error: " << e.what() << "\n";
                }

                break;
            }
            case 23:
                st.showBusiestDayStatistics();
                break;
            case 24: {
                std::string cName, date;
                int mins;
                std::cout << "Enter course name: ";
                std::cin >> cName;
                std::cout << "Enter duration (minutes): ";
                std::cin >> mins;
                std::cout << "Enter date (YYYY-MM-DD): ";
                std::cin >> date;

                st.logStudySession(cName, mins, date);
                break;
            }

            case 25:
                st.showSessionHistory();
                break;
            default: std::cout << "Invalid option.\n";
                break;
        }
    }
}