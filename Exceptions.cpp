#include "Exceptions.h"

StudyException::StudyException(const std::string& msg)
    : message_("StudyTracker Error: " + msg) {
}

const char* StudyException::what() const noexcept {
    return message_.c_str();
}

InvalidInputException::InvalidInputException(const std::string& msg)
    : StudyException("Input Invalid -> " + msg) {
}

DateException::DateException(const std::string& dateStr)
    : StudyException("Format data incorect sau in trecut: " + dateStr) {
}

FileException::FileException(const std::string& filename)
    : StudyException("Problema critica la fisierul: " + filename) {
}