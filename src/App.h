#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>

class App
{
private:
    SDL_Window* window;
    GLint w, h;
    SDL_GLContext context;
public:
    App();
    App(const App& _) = delete;
    void operator=(const App& _) = delete;
    ~App();
    int run();
};
