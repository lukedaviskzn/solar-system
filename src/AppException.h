#pragma once

#include <exception>
#include <string>

class AppException: public std::exception
{
private:
    std::string _message;
public:
    AppException(std::string message) : _message(message) {}
    ~AppException() throw() {}
    const char* what() const throw() {
        return _message.c_str();
    }
};
