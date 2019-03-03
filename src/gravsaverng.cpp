#include <iostream>
#include <SFML/Graphics.hpp>
#include <array>
#include <thread>
#include <chrono>
#include <random>

#include "StopWatch.h"
#include "rng.h"
#include "GravSaverNG.h"
#include "utils.h"

namespace {
    int S_WID;
    int S_HEI;
    float S_WID_SCALE;
    float S_HEI_SCALE;
    float SCALE;
    const int N_ASTEROIDS = 30000;
    const int N_PLANETS = 6;
    const float ASTEROID_MASS = 100.f;
    const float PLANET_MASS_MIN = 16000.f;
    const float PLANET_MASS_MAX = PLANET_MASS_MIN + 8000.f;
    const float PI = 3.141592653589793f;
    const int ASTEROIDS_LEFT_BEFORE_RESET = 1500;
    const float PHYSICS_SPEED = 4.f;
    const float DARKENING_SPEED = 24.f;
    const float COLOR_MULTIPLIER = 10.f;
    const int FPS_LIMIT = 0;
    const auto WAIT_BEFORE_RESET = std::chrono::seconds(0);

    // array<<px1, py1>, <px2, py2>> + color information etc.
    sf::VertexArray asteroidVertexes{ sf::Lines, N_ASTEROIDS * 2 };

    // array<dx, dy>
    std::array<sf::Vector2f, N_ASTEROIDS> asteroidVelocities;
    std::array<bool, N_ASTEROIDS> asteroidIsAlive;
    size_t numAsteroidsAlive = 0;

    // array<px, py, mass>
    // mass should be chosen by taking a uniform random between MIN and MAX
    std::array<sf::Vector3f, N_PLANETS> planets;

    float deltaT = 0;
    bool resetPending = false;
    std::chrono::time_point<std::chrono::steady_clock> timeOfReset;
}

namespace {
    bool rightDistanceFromPlanets(const sf::Vector2f& pos) {
        float minDistance = 10000000000000.f;
        for (auto& p : planets) {
            auto d = pos - sf::Vector2f{p.x, p.y};
            minDistance = std::min(std::sqrt((d.x*d.x) + (d.y*d.y)), minDistance);
        }

        return minDistance > S_WID*.1 && minDistance < S_WID*.15;
    }
    void reset() {
        for (auto& p : planets) {
            p.x = uniformRand(200 * 2, (1920 - 200) * (S_HEI_SCALE));
            p.y = uniformRand(200 * 2, (1080 - 200)* (S_HEI_SCALE));
            p.z = uniformRand(PLANET_MASS_MIN, PLANET_MASS_MAX);
        }

        sf::Vector2f pos;
        do {
            pos = { uniformRand(0, S_WID), uniformRand(0, S_HEI) };
        } while (!rightDistanceFromPlanets(pos));

        for (int i = 0; i < N_ASTEROIDS; ++i) {
            if (!asteroidIsAlive[i]) {
                asteroidVertexes[i * 2].position = pos;
                asteroidVertexes[i * 2 + 1].position = pos;
                asteroidVertexes[i * 2].color = sf::Color::Transparent;
                asteroidVertexes[i * 2 + 1].color = sf::Color::Transparent;
                auto v = uniformRand(-1, 1) * 2 - 3;
                auto a = uniformRand(0, 2 * PI);
                asteroidVelocities[i] = { v * cos(a), v * sin(a) };
                asteroidIsAlive[i] = true;
                ++numAsteroidsAlive;
            }
        }
    }

    bool isOutOfBounds(const sf::Vector2f& a) {
        auto frame = 20 * SCALE;

        return a.x < (-frame) || a.x >(S_WID + frame) || a.y < (-frame) || a.y > (S_HEI + frame);
    }

    float calcDistance(const sf::Vector2f& a, const sf::Vector3f& p) {
        auto dx = a.x - p.x;
        auto dy = a.y - p.y;
        return sqrt(dx*dx + dy * dy) / SCALE;
    }

    bool hitsPlanet(const sf::Vector2f& a, const sf::Vector3f& p) {
        auto distance = calcDistance(a, p);
        static auto radiusA = sqrt(ASTEROID_MASS / PI) / SCALE;
        auto radiusP = sqrt(p.z / PI) / SCALE;
        return distance < (radiusA + radiusP) / 1.3f;
    }

    float calcAngle(const sf::Vector2f& a, const sf::Vector3f& p) {
        return atan2(p.x - a.x, p.y - a.y);
    }
}

int main()
{
    auto videoMode = sf::VideoMode::getFullscreenModes().front();
    videoMode.width = 1920 * 2;
    videoMode.height = 1080 * 2;
    videoMode.bitsPerPixel = 32;
    sf::RenderWindow window(videoMode, "SFML window", sf::Style::Fullscreen);
    S_WID = 1920*2;
    S_HEI = 1080*2;
    S_WID_SCALE = S_WID / 1920.f;
    S_HEI_SCALE = S_HEI / 1080.f;
    SCALE = std::max(S_WID_SCALE, S_HEI_SCALE);

    reset();
    
    window.setFramerateLimit(FPS_LIMIT);
    window.setVerticalSyncEnabled(false);

    sf::RenderTexture renderTexture;
    {
        sf::ContextSettings settings;
        settings.antialiasingLevel = 0;
        if (!renderTexture.create(S_WID, S_HEI, settings)) {
            return -1;
        }
    }
    renderTexture.setSmooth(false);
    renderTexture.clear(sf::Color::Black);
    renderTexture.display();


    StopWatch swFrame("full frame");
    static int frame = 0;
    static auto frameTime = std::chrono::steady_clock::now();
    static size_t maxfps = 0;
    while (window.isOpen()) {
        if (++frame % 10 == 0 ) {
            auto now = std::chrono::steady_clock::now();
            auto delta = now - frameTime;
            auto deltaSeconds = delta.count() / 1e9f;
            std::cout << 10.f / deltaSeconds << std::endl;
            frameTime = now;
        }
        swFrame.Tick();
        StopWatch sw("main loop");
        SetDeltaT();

        window.clear(sf::Color::Black);

        DarkenScreen(renderTexture);

        // update draw the frame
        // most of this can be done in shaders? Can I avoid copying any data to the CPU?
        // I fear sfml might make it hard without dropping down into opengl
        Update();

        // draw to window
        DrawToWindow(renderTexture, window);

        // handle events
        HandleEvents(window);
    }
    return 0;
}

void SetDeltaT()
{
    static std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameT = std::chrono::high_resolution_clock::now();
    const auto now = std::chrono::high_resolution_clock::now();
    deltaT = (now - lastFrameT).count() / 1e9f;
    lastFrameT = now;
}

void Update()
{
    StopWatch sw("update and draw frame");

    // make the "tails" of dead asteroids transparent
    // when they died we made only the "heads" transparent to make it less jarring
    for (int a = 0; a < N_ASTEROIDS; ++a) {
        if (!asteroidIsAlive[a]) {
            asteroidVertexes[a * 2].color = sf::Color::Transparent;
            asteroidVertexes[a * 2 + 1].color = sf::Color::Transparent;
        }
    }

    // if they're OOB mark them as dead
    for (int a = 0; a < N_ASTEROIDS; ++a) {
        if (asteroidIsAlive[a]) {
            if (isOutOfBounds(asteroidVertexes[a * 2].position)) {
                asteroidIsAlive[a] = false;
                --numAsteroidsAlive;
                asteroidVertexes[a * 2].color = sf::Color::Transparent;
            }
        }
    }

    // copy over the current position to the "previous" position
    for (int a = 0; a < N_ASTEROIDS; ++a) {
        asteroidVertexes[a * 2 + 1] = asteroidVertexes[a * 2];
    }

    // main gravity calculation loop
    for (int a = 0; a < N_ASTEROIDS; ++a) {
        if (asteroidIsAlive[a]) {
            for (int p = 0; p < N_PLANETS; ++p) {
                const auto& planet = planets[p];
                auto& asteroid = asteroidVertexes[a * 2];
                if (hitsPlanet(asteroid.position, planet)) {
                    asteroidIsAlive[a] = false;
                    --numAsteroidsAlive;
                    asteroid.color = sf::Color::Transparent;
                    break;
                }

                auto g = 1.f;
                auto dist = calcDistance(asteroid.position, planet);
                auto force = g * planet.z / (dist*dist);
                auto ax = force * (planet.x - asteroid.position.x) / dist;
                auto ay = force * (planet.y - asteroid.position.y) / dist;
                asteroidVelocities[a] += sf::Vector2f{ ax, ay } * (deltaT * PHYSICS_SPEED);
                asteroid.position += asteroidVelocities[a] * (deltaT * PHYSICS_SPEED);
            }
        }
    }

    // assign colors to the asteroids
    for (int a = 0; a < N_ASTEROIDS; ++a) {
        if (asteroidIsAlive[a]) {
            auto redf = ((asteroidVelocities[a].x * asteroidVelocities[a].x + asteroidVelocities[a].y * asteroidVelocities[a].y) / COLOR_MULTIPLIER) / 150.f;
            auto red = (redf > 1.f ? 1.f : redf);
            auto green = 40.f/255.f;
            auto blue = 80.f/255.f;
            auto alpha = 170.f/255.f;
            asteroidVertexes[a * 2].color = { red, green, blue, alpha };
        }
    }

    { // check to see how many asteroids are alive, and if its under a limit, generate new ones in a new spot
        if (resetPending && std::chrono::steady_clock::now() > timeOfReset) {
            reset();
            resetPending = false;
        }
    }
}

void DarkenScreen(sf::RenderTexture &renderTexture)
{
    if(!resetPending && numAsteroidsAlive > N_ASTEROIDS * 0.1f)
    {
        StopWatch sw("darken the screen");
        static std::string darkenigShaderText =                                                                 \
            "uniform sampler2D texture;"                                                                        \
            "uniform float dt;"                                                                                 \
            "void main()"                                                                                       \
            "{"                                                                                                 \
                "vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);"                                           \
                "vec4 darkeningVec = vec4(4.f/255.f, 4.f/255.f, 4.f/255.f, 0);"                                 \
                "vec4 darkeningVec2 = darkeningVec * dt;"                                                       \
                "vec4 newPixel = pixel - darkeningVec2;"                                                        \
                "gl_FragColor = clamp(vec4(newPixel.xyz, 1), 0, 1);"                                            \
            "}"                                                                                                 \
            ;

        static sf::Shader darkeningShader;
        static bool firstLoad = true;
        if (firstLoad) {
            if (!darkeningShader.loadFromMemory(darkenigShaderText, sf::Shader::Fragment)) {
                std::cerr << "COULD NOT LOAD SHADER" << std::endl;
                exit(EXIT_FAILURE);
            }
            firstLoad = false;
            darkeningShader.setUniform("texture", sf::Shader::CurrentTexture);
        }
        darkeningShader.setUniform("dt", deltaT * DARKENING_SPEED);

        const auto& currentTexture = renderTexture.getTexture();
        sf::Sprite currentSprite(currentTexture);

        sf::RenderStates renderStates = &darkeningShader;
        renderStates.blendMode = sf::BlendNone;

        renderTexture.draw(currentSprite, renderStates);
    } else {
        if (!resetPending) {
            timeOfReset = std::chrono::steady_clock::now() + WAIT_BEFORE_RESET;
            resetPending = true;
        }
    }
}

void DrawToWindow(sf::RenderTexture &renderTexture, sf::RenderWindow &window)
{
    StopWatch sw("dispatch draw calls");
    {
        StopWatch sw("draw to asteroids");
        renderTexture.draw(asteroidVertexes);
    }
    {
        StopWatch sw("render off screen");
        renderTexture.display();
    }
    {
        StopWatch sw("draw sprite");
        sf::Sprite sprite(renderTexture.getTexture());
        window.clear();
        window.draw(sprite);
    }
    {
        StopWatch sw("render window");
        window.display();
    }
}

void HandleEvents(sf::RenderWindow &window)
{
    StopWatch sw("handle events");
    sf::Event e;
    while (window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) {
            window.close();
        }
        if (e.type == sf::Event::KeyReleased) {
            if (e.key.code == sf::Keyboard::Key::Escape) {
                window.close();
            }
        }
    }
}
