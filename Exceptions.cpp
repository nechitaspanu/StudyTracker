#include "Exceptions.h"

StudyException::StudyException(const std::string& msg)
    : message_("StudyTracker Error: " + msg) {
}

const char* StudyException::what() const noexcept {
    return message_.c_str();
}

InvalidInputException::InvalidInputException(const std::string& msg)
    : StudyException("Invalid Input -> " + msg) {
}

DateException::DateException(const std::string& dateStr)
    : StudyException("Incorrect format data (or in the past): " + dateStr) {
}

FileException::FileException(const std::string& filename)
    : StudyException("Critical file error: " + filename) {
}