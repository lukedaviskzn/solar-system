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
#include "Sky.h"

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
    
    meshFile = std::ifstream("res/ring_plane.obj");
    std::shared_ptr<Mesh> ring_mesh = Mesh::LoadObj(meshFile, false);

    // all distances in solar radii
    float sun_size = 1.0f;
    float sun_rotate_speed = 1.0f / 27.0f;
    float sun_axial_tilt = 0.1265f;
    int sun_num_shells = 40;

    float earth_size = 1.0 / 109.0;
    float earth_orbit_dist = 215.87;
    float earth_orbit_speed = 1.0/365.25;
    float earth_rotate_speed = 1.0/24.0;
    float earth_axial_tilt = 0.4091;
    float earth_cloud_dist = 1.008;
    float earth_cloud_rotate_speed = earth_rotate_speed * 0.85; // slower, so moving west
    int earth_num_shells = 20;

    float moon_size = earth_size * 0.2731;
    float moon_orbit_dist = 0.55203 / 5.0; // moon moved closer for aesthetics
    float moon_orbit_speed = 1.0f/27.0;
    float moon_rotate_speed = moon_orbit_speed;
    float moon_axial_tilt = 0.1166;

    float mercury_size = 1.0 / 285.5;
    float mercury_orbit_dist = 83.15;
    float mercury_orbit_speed = 1.0 / 88.0;
    float mercury_rotate_speed = 1.0 / 176.0;
    float mercury_axial_tilt = 0.0;

    float mars_size = 1.0 / 205.44;
    float mars_orbit_dist = 327.43;
    float mars_orbit_speed = 1.0 / 687.0;
    float mars_rotate_speed = 1.0 / 1.026;
    float mars_axial_tilt = 0.0614;
    int mars_num_shells = 10;

    float jupiter_size = 1.0 / 9.96;
    float jupiter_orbit_dist = 1119.42;
    float jupiter_orbit_speed = 1.0 / 4333.0;
    float jupiter_rotate_speed = 1.0 / 0.41389;
    float jupiter_axial_tilt = 0.054629;
    int jupiter_num_shells = 10;

    float venus_size = 1.0 / 115.0;
    float venus_orbit_dist = 156.06;
    float venus_orbit_speed = 1.0/225.0;
    float venus_rotate_speed = 1.0/243.0;
    float venus_axial_tilt = 0.4353;
    int venus_num_shells = 10;

    float saturn_size = 1.0 / 11.958;
    float saturn_orbit_dist = 2083.465;
    float saturn_orbit_speed = 1.0 / 365.25 / 29.4;
    float saturn_rotate_speed = 1.0 / 0.4403;
    float saturn_axial_tilt = 0.4665;
    float saturn_ring_rotate_speed = 1.0 / 0.8; // rough estimate as different rings travel at different speeds.
    float saturn_ring_radius = 0.20;
    int saturn_num_shells = 10;

    float uranus_size = 1.0 / 27.456;
    float uranus_orbit_dist = 4208.15;
    float uranus_orbit_speed = 1.0 / 30687;
    float uranus_rotate_speed = 1.0 / 0.7181;
    float uranus_axial_tilt = 1.70641;
    int uranus_num_shells = 10;

    float neptune_size = 1.0 / 28.28;
    float neptune_orbit_dist = 6422.44;
    float neptune_orbit_speed = 1.0 / 165.0 / 365.25;
    float neptune_rotate_speed = 1.0 / 0.6708;
    float neptune_axial_tilt = 0.4887;
    int neptune_num_shells = 10;

    float camera_fov = M_PI / 6.0;

    
    std::shared_ptr<Texture> sky_tex = std::make_shared<Texture>("res/sky_8192.jpg");

    std::shared_ptr<Texture> sun_tex = std::make_shared<Texture>("res/sun_2048.jpg");
    std::shared_ptr<Texture> sun_atmos_tex = std::make_shared<Texture>(255, 200, 100, 2);

    std::shared_ptr<Texture> earth_tex = std::make_shared<Texture>("res/earth_4096.jpg");
    std::shared_ptr<Texture> earth_night_tex = std::make_shared<Texture>("res/earth_night_2048.jpg");
    std::shared_ptr<Texture> earth_atmos_tex = std::make_shared<Texture>(150, 205, 255, 25);
    std::shared_ptr<Texture> clouds_tex = std::make_shared<Texture>("res/clouds_8192.jpg");

    std::shared_ptr<Texture> moon_tex = std::make_shared<Texture>("res/moon_2048.jpg");
    
    std::shared_ptr<Texture> mercury_tex = std::make_shared<Texture>("res/mercury_2048.jpg");
    
    std::shared_ptr<Texture> mars_tex = std::make_shared<Texture>("res/mars_2048.jpg");
    std::shared_ptr<Texture> mars_atmos_tex = std::make_shared<Texture>(215, 205, 205, 5);
    
    std::shared_ptr<Texture> jupiter_tex = std::make_shared<Texture>("res/jupiter_2048.jpg");
    std::shared_ptr<Texture> jupiter_atmos_tex = std::make_shared<Texture>(250, 240, 230, 25);
    
    std::shared_ptr<Texture> venus_tex = std::make_shared<Texture>("res/venus_2048.jpg");
    std::shared_ptr<Texture> venus_atmos_tex = jupiter_atmos_tex;
    
    std::shared_ptr<Texture> saturn_tex = std::make_shared<Texture>("res/saturn_2048.jpg");
    std::shared_ptr<Texture> saturn_ring_tex = std::make_shared<Texture>("res/saturn_ring_2048.png");
    std::shared_ptr<Texture> saturn_atmos_tex = jupiter_atmos_tex;
    
    std::shared_ptr<Texture> uranus_tex = std::make_shared<Texture>("res/uranus_2048.jpg");
    std::shared_ptr<Texture> uranus_atmos_tex = earth_atmos_tex;
    
    std::shared_ptr<Texture> neptune_tex = std::make_shared<Texture>("res/neptune_2048.jpg");
    std::shared_ptr<Texture> neptune_atmos_tex = earth_atmos_tex;

    meshFile.close();

    std::vector<GameObject*> atmosphere;
    // draw in order outer to inner, so depth doesn't block later draws
    for (size_t i = sun_num_shells; i >= 1; i--) {
        float shell_radius = i * i * 10.0f / sun_num_shells / sun_num_shells + 1.0f;
        atmosphere.push_back(new StellarBody(sun_atmos_tex, 0, inv_sphere_mesh, sun_size * shell_radius, 0.0f, 0.0f));
    }
    
    std::vector<GameObject*> earth_atmosphere;
    for (size_t i = earth_num_shells; i >= 1; i--) {
        float shell_radius = i * i * 0.05f / earth_num_shells / earth_num_shells + 1.0f;
        earth_atmosphere.push_back(new StellarBody(earth_atmos_tex, 3, inv_sphere_mesh, earth_size * shell_radius, 0.0f, 0.0f));
    }
    // earth clouds
    earth_atmosphere.push_back(new StellarBody(clouds_tex, 2, sphere_mesh, earth_size * earth_cloud_dist, earth_cloud_rotate_speed, earth_axial_tilt));
    
    std::vector<GameObject*> jupiter_atmosphere;
    for (size_t i = jupiter_num_shells; i >= 1; i--) {
        float shell_radius = i * i * 0.05f / jupiter_num_shells / jupiter_num_shells + 1.0f;
        jupiter_atmosphere.push_back(new StellarBody(jupiter_atmos_tex, 3, inv_sphere_mesh, jupiter_size * shell_radius, 0.0f, 0.0f));
    }
    
    std::vector<GameObject*> venus_atmosphere;
    for (size_t i = venus_num_shells; i >= 1; i--) {
        float shell_radius = i * i * 0.05f / venus_num_shells / venus_num_shells + 1.0f;
        venus_atmosphere.push_back(new StellarBody(venus_atmos_tex, 3, inv_sphere_mesh, venus_size * shell_radius, 0.0f, 0.0f));
    }
    
    std::vector<GameObject*> saturn_atmosphere;
    // saturn ring
    saturn_atmosphere.push_back(new StellarBody(saturn_ring_tex, 5, ring_mesh, saturn_ring_radius, saturn_ring_rotate_speed, saturn_axial_tilt));
    for (size_t i = saturn_num_shells; i >= 1; i--) {
        float shell_radius = i * i * 0.05f / saturn_num_shells / saturn_num_shells + 1.0f;
        saturn_atmosphere.push_back(new StellarBody(saturn_atmos_tex, 3, inv_sphere_mesh, saturn_size * shell_radius, 0.0f, 0.0f));
    }
    
    std::vector<GameObject*> uranus_atmosphere;
    for (size_t i = uranus_num_shells; i >= 1; i--) {
        float shell_radius = i * i * 0.05f / uranus_num_shells / uranus_num_shells + 1.0f;
        uranus_atmosphere.push_back(new StellarBody(uranus_atmos_tex, 3, inv_sphere_mesh, uranus_size * shell_radius, 0.0f, 0.0f));
    }
    
    std::vector<GameObject*> neptune_atmosphere;
    for (size_t i = neptune_num_shells; i >= 1; i--) {
        float shell_radius = i * i * 0.05f / neptune_num_shells / neptune_num_shells + 1.0f;
        neptune_atmosphere.push_back(new StellarBody(neptune_atmos_tex, 3, inv_sphere_mesh, neptune_size * shell_radius, 0.0f, 0.0f));
    }
    
    atmosphere.push_back(
        // earth orbit
        new Orbit(earth_orbit_dist, earth_orbit_speed, earth_atmosphere)
    );
    
    std::vector<GameObject*> mars_atmosphere;
    for (size_t i = mars_num_shells; i >= 1; i--) {
        float shell_radius = i * i * 0.05f / mars_num_shells / mars_num_shells + 1.0f;
        mars_atmosphere.push_back(new StellarBody(mars_atmos_tex, 3, inv_sphere_mesh, mars_size * shell_radius, 0.0f, 0.0f));
    }

    atmosphere.push_back(
        // mars orbit
        new Orbit(mars_orbit_dist, mars_orbit_speed, mars_atmosphere)
    );

    atmosphere.push_back(
        // jupiter orbit
        new Orbit(jupiter_orbit_dist, jupiter_orbit_speed, jupiter_atmosphere)
    );

    atmosphere.push_back(
        // venus orbit
        new Orbit(venus_orbit_dist, venus_orbit_speed, venus_atmosphere)
    );

    atmosphere.push_back(
        // saturn orbit
        new Orbit(saturn_orbit_dist, saturn_orbit_speed, saturn_atmosphere)
    );

    atmosphere.push_back(
        // uranus orbit
        new Orbit(uranus_orbit_dist, uranus_orbit_speed, uranus_atmosphere)
    );

    atmosphere.push_back(
        // neptune orbit
        new Orbit(neptune_orbit_dist, neptune_orbit_speed, neptune_atmosphere)
    );
    
    GameObject* root = new Scene({
        // sun orbit (stationary)
        new Orbit(0.0f, 0.0f, {
            // sky
            new Sky(sky_tex, inv_sphere_mesh, 1000.0f),
            // sun
            new StellarBody(sun_tex, 4, sphere_mesh, sun_size, sun_rotate_speed, sun_axial_tilt),
            // sun camera
            new Camera(0, Transform({0.0, 0.0, 10.0}), camera_fov),
            // mercury orbit
            new Orbit(mercury_orbit_dist, mercury_orbit_speed, {
                // mercury camera
                new Camera(1, Transform({0.0, 0.0, 0.1}), camera_fov),
                // mercury
                new StellarBody(mercury_tex, 1, sphere_mesh, mercury_size, mercury_rotate_speed, mercury_axial_tilt),
            }),
            // venus orbit
            new Orbit(venus_orbit_dist, venus_orbit_speed, {
                // venus camera
                new Camera(2, Transform({0.0, 0.0, 0.1}), camera_fov),
                // venus
                new StellarBody(venus_tex, 1, sphere_mesh, venus_size, venus_rotate_speed, venus_axial_tilt),
            }),
            // earth orbit
            new Orbit(earth_orbit_dist, earth_orbit_speed, {
                // earth camera
                new Camera(3, Transform({0.0, 0.0, 0.1}), camera_fov),
                // earth
                new StellarBody(earth_tex, earth_night_tex, sphere_mesh, earth_size, earth_rotate_speed, earth_axial_tilt),
                // moon orbit
                new Orbit(moon_orbit_dist, moon_orbit_speed, {
                    // moon camera
                    new Camera(4, Transform({0.0, 0.0, 0.05}), camera_fov),
                    // moon
                    new StellarBody(moon_tex, 1, sphere_mesh, moon_size, moon_rotate_speed, moon_axial_tilt),
                }),
            }),
            // mars orbit
            new Orbit(mars_orbit_dist, mars_orbit_speed, {
                // mercury camera
                new Camera(5, Transform({0.0, 0.0, 0.1}), camera_fov),
                // mercury
                new StellarBody(mars_tex, 1, sphere_mesh, mars_size, mars_rotate_speed, mars_axial_tilt),
            }),
            // jupiter orbit
            new Orbit(jupiter_orbit_dist, jupiter_orbit_speed, {
                // jupiter camera
                new Camera(6, Transform({0.0, 0.0, 1.0}), camera_fov),
                // jupiter
                new StellarBody(jupiter_tex, 1, sphere_mesh, jupiter_size, jupiter_rotate_speed, jupiter_axial_tilt),
            }),
            // saturn orbit
            new Orbit(saturn_orbit_dist, saturn_orbit_speed, {
                // saturn camera
                new Camera(7, Transform({0.0, 0.0, 1.0}), camera_fov),
                // saturn
                new StellarBody(saturn_tex, 1, sphere_mesh, saturn_size, saturn_rotate_speed, saturn_axial_tilt),
            }),
            // uranus orbit
            new Orbit(uranus_orbit_dist, uranus_orbit_speed, {
                // uranus camera
                new Camera(8, Transform({0.0, 0.0, 0.8}), camera_fov),
                // uranus
                new StellarBody(uranus_tex, 1, sphere_mesh, uranus_size, uranus_rotate_speed, uranus_axial_tilt),
            }),
            // neptune orbit
            new Orbit(neptune_orbit_dist, neptune_orbit_speed, {
                // neptune camera
                new Camera(9, Transform({0.0, 0.0, 0.8}), camera_fov),
                // neptune
                new StellarBody(neptune_tex, 1, sphere_mesh, neptune_size, neptune_rotate_speed, neptune_axial_tilt),
            }),
        }),
        // draw transparent objects last
        // parallel orbits for transparencies
        new Orbit(0.0f, 0.0f, atmosphere),
    });

    UpdateContext ctx = {.dt = 0.0f, .aspect = w*1.0f/h, .active_camera = 0};

    int32_t mouse_dx = 0;
    int32_t mouse_dy = 0;
    int32_t physics_mouse_dx = 0;
    int32_t physics_mouse_dy = 0;
    
    bool paused = false;
    float speedup = 1.0f;

    size_t frame_count = 0;
    size_t average_dt_count = 100;
    float average_dt = 0.0f;

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
                case SDLK_q:
                    running = false;
                    break;
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
                case SDLK_n:
                    ctx.active_camera = (ctx.active_camera+1) % 10;
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
        average_dt += dt;

        if (frame_count % average_dt_count == 0) {
            average_dt /= average_dt_count;
            std::cout << "FPS " << 1.0f / average_dt << ", DT " << average_dt << std::endl;
        }

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
        root->prerender(shader, ctx);
        root->render(shader, ctx);

        GLenum error;
        if ((error = glGetError()) != GL_NO_ERROR) {
            throw(GlException("drawing", error));
        }

        SDL_GL_SwapWindow(window);

        frame_count++;
    }

    return EXIT_SUCCESS;
}
