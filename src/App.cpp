#include "App.h"

#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "AppException.h"
#include "GlException.h"
#include "Mesh.h"
#include "Shader.h"
#include "GameObject.h"
#include "Orbit.h"
#include "StellarBody.h"
#include "Camera.h"
#include "Scene.h"
#include "Texture.h"

App::App() : w(800), h(600) {
    SDL_Init(SDL_INIT_EVERYTHING);
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

    std::string title = "Solar System";

    window = SDL_CreateWindow(title.c_str(), 100, 100, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        throw AppException(SDL_GetError());
    }

    context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        throw AppException(SDL_GetError());
    }

    if (GLEW_OK != glewInit()) {
        throw AppException("Failed to initialise GLEW.");
    }

    glViewport(0, 0, w, h);
}

App::~App() {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int App::run() {
    SDL_Event evt;

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);

    std::ifstream vertFile("res/shader.vert.glsl");
    std::ifstream fragFile("res/shader.frag.glsl");
    
    Shader shader(vertFile, fragFile);
    // Shader shader("#version 330 core\nlayout (location = 0) in vec3 position;\nuniform mat4 model;\nuniform mat4 view;\nuniform mat4 perspective;\n\nvoid main() {\ngl_Position = perspective * view * model * vec4(position, 1.0);\n}\n", "#version 330 core\nout vec4 FragColor;\nvoid main() {\nFragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n}");

    vertFile.close();
    fragFile.close();

    shader.use();

    bool running = true;

    // auto due to template nonsense
    auto current_time = std::chrono::steady_clock::now();
    auto last_time = current_time;

    float physics_dt = 0.0;
    float physics_target = 1.0f/60.0f;

    std::ifstream meshFile("res/high_def_sphere.obj");
    std::shared_ptr<Mesh> sphere_mesh = Mesh::LoadObj(meshFile);
    meshFile = std::ifstream("res/high_def_sphere.obj");
    std::shared_ptr<Mesh> inv_sphere_mesh = Mesh::LoadObj(meshFile, true);

    float earth_orbit_dist = 107.9f;
    float moon_orbit_dist = 0.2757f / 5.0;
    float sun_size = 1.0f;
    float earth_size = sun_size / 109.0f;
    float moon_size = earth_size * 0.2731f;
    
    std::shared_ptr<Texture> earth_tex = std::make_shared<Texture>("res/earth_4096.jpg");
    std::shared_ptr<Texture> earth_night_tex = std::make_shared<Texture>("res/earth_night_2048.jpg");
    std::shared_ptr<Texture> sun_tex = std::make_shared<Texture>("res/sun_2048.jpg");
    std::shared_ptr<Texture> moon_tex = std::make_shared<Texture>("res/moon_2048.jpg");
    std::shared_ptr<Texture> sky_tex = std::make_shared<Texture>("res/sky_8192.jpg");
    std::shared_ptr<Texture> clouds_tex = std::make_shared<Texture>("res/clouds_8192.jpg");
    std::shared_ptr<Texture> earth_atmos_tex = std::make_shared<Texture>(150, 205, 255, 25);
    std::shared_ptr<Texture> sun_atmos_tex = std::make_shared<Texture>(255, 200, 100, 2);
    meshFile.close();

    std::vector<GameObject*> atmosphere;
    // draw in order outer to inner, so depth doesn't block later draws
    for (size_t i = 100; i >= 1; i--) {
        float shell_radius = i * i / 2000.0f + 1.0f;
        atmosphere.push_back(new StellarBody(sun_atmos_tex, 0, inv_sphere_mesh, sun_size * shell_radius, 0.0f, 0.0f));
    }
    
    std::vector<GameObject*> earth_atmosphere;
    for (size_t i = 20; i >= 1; i--) {
        float shell_radius = i * i / 6000.0f + 1.0f;
        earth_atmosphere.push_back(new StellarBody(earth_atmos_tex, 3, inv_sphere_mesh, earth_size * shell_radius, 0.0f, 0.0f));
    }
    // earth clouds
    earth_atmosphere.push_back(new StellarBody(clouds_tex, 2, sphere_mesh, earth_size * 1.008f, 0.08f, 0.4091f));
    
    atmosphere.push_back(
        // earth orbit
        new Orbit(earth_orbit_dist, 0.025f, earth_atmosphere)
    );
    
    GameObject* root = new Scene({
        // sun orbit (stationary)
        new Orbit(0.0f, 0.0f, {
            // sky
            new StellarBody(sky_tex, 0, inv_sphere_mesh, 1000.0f, 0.0f, 0.0f),
            // sun
            new StellarBody(sun_tex, 4, sphere_mesh, sun_size, 0.1f, 0.1265f),
            // earth orbit
            new Orbit(earth_orbit_dist, 0.025f, {
                new Camera(Transform({0.0f, 0.0f, 0.1f}), M_PI / 6.0),
                // earth
                new StellarBody(earth_tex, earth_night_tex, sphere_mesh, earth_size, 0.1f, 0.4091f),
                // moon orbit
                new Orbit(moon_orbit_dist, 0.1f, {
                    // moon
                    new StellarBody(moon_tex, 1, sphere_mesh, moon_size, 0.1f, 0.1166f),
                }),
            }),
        }),
        // draw transparent objects last
        // parallel orbits for transparencies
        new Orbit(0.0f, 0.0f, atmosphere),
    });

    UpdateContext ctx = {.dt = 0.0f, .aspect = w*1.0f/h};

    int32_t mouse_dx = 0;
    int32_t mouse_dy = 0;
    int32_t physics_mouse_dx = 0;
    int32_t physics_mouse_dy = 0;

    bool paused = false;
    float speedup = 1.0f;
    
    while (running) {
        while (SDL_PollEvent(&evt)) {
            switch (evt.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_WINDOWEVENT:
                if (evt.window.event == SDL_WINDOWEVENT_RESIZED) {
                    w = evt.window.data1;
                    h = evt.window.data2;
                    glViewport(0, 0, w, h);
                    ctx.aspect = w*1.0f/h;
                }
                break;
            case SDL_KEYDOWN:
                ctx.keys_pressed[evt.key.keysym.sym] = true;
                break;
            case SDL_KEYUP:
                ctx.keys_pressed[evt.key.keysym.sym] = false;
                
                switch (evt.key.keysym.sym) {
                case SDLK_ESCAPE:
                    SDL_SetWindowGrab(window, SDL_FALSE);
                    SDL_ShowCursor(SDL_ENABLE);
                    ctx.grabbed = false;
                    break;
                case SDLK_p:
                    paused = !paused;
                    break;
                case SDLK_UP:
                    speedup *= 1.5f;
                    break;
                case SDLK_DOWN:
                    speedup /= 1.5f;
                    break;
                case SDLK_q:
                    running = false;
                    break;
                }
                break;
            case SDL_MOUSEMOTION:
                mouse_dx += evt.motion.xrel;
                mouse_dy += evt.motion.yrel;
                physics_mouse_dx += evt.motion.xrel;
                physics_mouse_dy += evt.motion.yrel;
                break;
            case SDL_MOUSEBUTTONDOWN:
                ctx.grabbed = evt.button.button == 1;
                SDL_SetWindowGrab(window, SDL_TRUE);
                SDL_ShowCursor(SDL_DISABLE);
                break;
            }
        }
        if (!running) break;

        if (ctx.grabbed) {
            SDL_WarpMouseInWindow(window, w / 2, h / 2);
        }

        current_time = std::chrono::steady_clock::now();
        std::chrono::nanoseconds dt_nano = current_time - last_time;
        last_time = current_time;
        float dt = dt_nano.count() / 1.0e9;
        physics_dt += dt;

        ctx.dt = dt;

        ctx.mouse_dx = mouse_dx;
        ctx.mouse_dy = mouse_dy;

        root->update(ctx);

        mouse_dx = 0;
        mouse_dy = 0;

        while (physics_dt >= physics_target) {
            ctx.dt = physics_target * speedup;
            ctx.mouse_dx = physics_mouse_dx;
            ctx.mouse_dy = physics_mouse_dy;

            if (!paused) root->physics_update(ctx);
            
            physics_dt -= physics_target;

            physics_mouse_dx = 0;
            physics_mouse_dy = 0;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        root->update_transform(glm::mat4(1.0));
        root->prerender(shader);
        root->render(shader);

        GLenum error;
        if ((error = glGetError()) != GL_NO_ERROR) {
            throw(GlException("drawing", error));
        }

        SDL_GL_SwapWindow(window);
    }

    return EXIT_SUCCESS;
}
