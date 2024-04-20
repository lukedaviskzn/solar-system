#include <iostream>
#include "App.h"
#include "AppException.h"

int main(int argc, char** argv) {
    try {
        App app;
        return app.run();
    } catch (AppException e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
