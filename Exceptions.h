#ifndef OOP_EXCEPTIONS_H
#define OOP_EXCEPTIONS_H

#pragma once
#include <exception>
#include <string>

class StudyException : public std::exception {
protected:
    std::string message_;
public:
    explicit StudyException(const std::string& msg);

    const char* what() const noexcept override;
};

class InvalidInputException : public StudyException {
public:
    explicit InvalidInputException(const std::string& msg);
};

class DateException : public StudyException {
public:
    explicit DateException(const std::string& dateStr);
};

class FileException : public StudyException {
public:
    explicit FileException(const std::string& filename);
};

#endif //OOP_EXCEPTIONS_H