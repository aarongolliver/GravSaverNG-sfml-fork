#include "shader_tester.h"

#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <filesystem>
#include <SFML/Graphics.hpp>

#include "shader_header.h"

static auto startT = std::chrono::steady_clock::now();

namespace fs = std::filesystem;

void _main(std::vector<std::string> args) {
    FW::FileWatcher fw;
    bool changed = false;
    auto f = [&]() {
        changed = true;
    };
    LambdaWatcher<decltype(f)> watcher(f);
    fw.addWatch(args[1], &watcher);
    auto check_changed = [&]() {
        fw.update();
        if (changed)
        {
            changed = false;
            return true;
        }
        return false;
    };
    sf::RenderWindow window({ 1080, 1080}, "shader toy", sf::Style::Resize);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(0);

    sf::Shader shader;
    for (const auto & shaderFile : fs::directory_iterator(args[1])) {
        if (!shader.loadFromMemory(shaderHeader + fractalShader, sf::Shader::Type::Fragment)) {
            std::cerr << "FAILED TO LOAD SHADER " << std::endl;
        }
        std::cout << "LOADED" << std::endl;
    }
    

    sf::RenderTexture tex;
    tex.create(window.getSize().x, window.getSize().y);
    sf::Glsl::Vec2 mousePos = { window.getSize().x/2.f, window.getSize().y/2.f };
    while (window.isOpen()) {
        window.clear();
        tex.clear();
        const auto& size = window.getSize();
        shader.setUniform("iTime", (std::chrono::steady_clock::now() - startT).count() * 1.f / std::chrono::steady_clock::period::den);
        shader.setUniform("iResolution", sf::Glsl::Vec2(size));
        shader.setUniform("iMouse", mousePos);

        const auto& currentTexture = tex.getTexture();
        sf::Sprite currentSprite(currentTexture);
        sf::RenderStates renderStates(&shader);
        renderStates.blendMode = { sf::BlendMode::One, sf::BlendMode::One };
        tex.draw(currentSprite, renderStates);
        tex.display();
        window.draw(currentSprite);

        window.display();
        static bool mouseEnabled = false;
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
            if (e.type == sf::Event::Resized) {
                window.setView({ {e.size.width / 2.f, e.size.height / 2.f}, {1.f*e.size.width, 1.f*e.size.height} });
                tex.create(e.size.width, e.size.height);
            }
            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Button::Left)
                mouseEnabled = true;
            if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Button::Left)
                mouseEnabled = false;
            if (e.type == sf::Event::MouseMoved) {
                if(mouseEnabled)
                mousePos = { e.mouseMove.x * 1.f, e.mouseMove.y *  1.f };
            }
        }
    }

}










int main(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    _main(std::move(args));

    return EXIT_SUCCESS;
}