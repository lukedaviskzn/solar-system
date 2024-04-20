#pragma once

#include <exception>
#include <string>
#include <GL/glew.h>
// #include <GL/glu.h>

class GlException: public std::exception
{
private:
    std::string message;
public:
    GlException(std::string context, GLenum error) {
        // const char* msg = (const char*)gluErrorString(error);
        // message = context + ": " + std::to_string(error) + " \"" + std::string(msg) + "\"";
        message = context + ": " + std::to_string(error);
    }
    ~GlException() throw() {}
    const char* what() const throw() {
        return message.c_str();
    }
};
