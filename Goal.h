#ifndef OOP_GOAL_H
#define OOP_GOAL_H

#pragma once
#include <string>
#include <iostream>

class Goal {
protected:
    std::string description_;

public:
    explicit Goal(std::string desc);
    virtual ~Goal() = default;
    virtual bool isAchieved() const = 0;
    virtual void print(std::ostream& os) const = 0;
    virtual Goal* clone() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Goal& g);

    const std::string& getDescription() const { return description_; }
};


class ReadingGoal : public Goal {
private:
    int pagesTarget_;
    int pagesRead_;
public:
    ReadingGoal(std::string desc, int target, int read);
    bool isAchieved() const override;
    void print(std::ostream& os) const override;

    Goal* clone() const override { return new ReadingGoal(*this); }

    int getTarget() const { return pagesTarget_; }
    int getRead() const { return pagesRead_; }
};

class TimeGoal : public Goal {
private:
    int minutesTarget_;
    int minutesDone_;
public:
    TimeGoal(std::string desc, int target, int done);
    bool isAchieved() const override;
    void print(std::ostream& os) const override;

    Goal* clone() const override { return new TimeGoal(*this); }

    int getTarget() const { return minutesTarget_; }
    int getDone() const { return minutesDone_; }
};

class ExamGoal : public Goal {
private:
    double gradeTarget_;
    double currentGrade_;
public:
    ExamGoal(std::string desc, double target, double current);
    bool isAchieved() const override;
    void print(std::ostream& os) const override;

    Goal* clone() const override { return new ExamGoal(*this); }

    double getTarget() const { return gradeTarget_; }
    double getCurrent() const { return currentGrade_; }
};

class SessionGoal : public Goal {
private:
    int targetSessions;
    int currentSessions;

public:
    SessionGoal(const std::string& desc, int target, int current = 0)
        : Goal(desc), targetSessions(target), currentSessions(current) {}

    bool isAchieved() const override {
        return currentSessions >= targetSessions;
    }

    Goal* clone() const override {
        return new SessionGoal(*this);
    }

    void incrementSession() {
        currentSessions++;
    }

    int getTarget() const { return targetSessions; }
    int getCurrent() const { return currentSessions; }

    void print(std::ostream& os) const override {
        os << "[Session Goal] " << description_ << ": "
           << currentSessions << "/" << targetSessions << " sessions logged";
    }
};

#endif //OOP_GOAL_H