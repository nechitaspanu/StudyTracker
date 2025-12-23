#ifndef OOP_STUDYTRACKER_H
#define OOP_STUDYTRACKER_H

#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <numeric>
#include "Course.h"
#include "Assignment.h"
#include "CalendarEvent.h"
#include "Goal.h"

struct StudySession {
    std::string courseName;
    int durationMinutes;
    std::string date;
};

struct Flashcard {
    std::string question;
    std::string answer;
};

class StudyTracker {
private:
    std::vector<Course> courses_;
    std::vector<Assignment> assignments_;
    std::vector<CalendarEvent> events_;
    std::vector<Goal*> goals_;
    std::vector<Flashcard> flashcards_;

    [[nodiscard]] const Course *findCourse(const std::string &name) const;

    static std::string keyCourse(const Course &c);
    static std::string keyAssignment(const Assignment &a);
    static std::string keyEvent(const CalendarEvent &e);

public:
    StudyTracker() = default;
    ~StudyTracker();

    std::vector<StudySession> sessions_;

    void flashcardMode();
    void addFlashcard();
    void runQuiz();

    StudyTracker(const StudyTracker& other);
    StudyTracker& operator=(const StudyTracker& other);

    void addGoal(Goal* g);
    void addCourse(const Course &c);
    void addAssignment(const Assignment &a);
    void addEvent(const CalendarEvent &e);
    void showBusiestDayStatistics() const;

    bool removeCourse(const std::string &name);
    bool removeAssignment(const std::string &title);
    bool removeEvent(const std::string &label);

    bool renameCourse(const std::string &oldName, const std::string &newName);
    bool setCourseUnits(const std::string &name, int total, int done);
    bool completeCourseUnits(const std::string &name, int units);

    bool editAssignmentTitle(const std::string &oldTitle, const std::string &newTitle);
    bool editAssignmentDue(const std::string &title, const std::string &newDueYMD);
    bool editAssignmentNotes(const std::string &title, const std::string &newNotes);

    bool renameEvent(const std::string &oldLabel, const std::string &newLabel);
    bool setEventDate(const std::string &label, const std::string &newDateYMD);

    bool addTagToCourse(const std::string &name, const std::string &tg);
    bool removeTagFromCourse(const std::string &name, const std::string &tg);
    std::vector<Course> filterByTag(const std::string &tg) const;

    void clearAll();
    void dedupe();
    [[nodiscard]] double overallProgress() const;

    [[nodiscard]] std::vector<Assignment> upcomingDeadlines(int days) const;
    [[nodiscard]] std::string courseReport(const std::string &name) const;

    void logStudySession(const std::string& course, int minutes, const std::string& date);
    void showSessionHistory() const;

    void calculateGradeNeeded() const;

    friend std::ostream &operator<<(std::ostream &os, const StudyTracker &st);
    friend void saveToFile(const StudyTracker &st, const std::string &path);
};

void saveToFile(const StudyTracker &st, const std::string &path);
void loadFromFiles(StudyTracker &st, const std::string &path);

#endif //OOP_STUDYTRACKER_H