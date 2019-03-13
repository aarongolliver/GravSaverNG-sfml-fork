#include "live_reloading_shader.h"

#include "SFML/Graphics.hpp"

#include <iostream>
#include <thread>
#include "shader_tester.h"
#include <chrono>

#include "shader_header.h"

using namespace std::chrono_literals;

static auto startT = std::chrono::steady_clock::now();

namespace {
    std::string SanatizeTextureName(const std::string& stem) {
        std::string ret;
        for (const auto& c : stem) {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')) {
                ret += c;
            }
        }
        return "texture_" + ret;
    }

    std::string GetTextureNames(const std::vector<std::pair<fs::path, std::unique_ptr<LiveReloadingShader>>>& shaders) {
        std::string header;
        for (const auto& shader : shaders) {
            header += "uniform sampler2D " + SanatizeTextureName(shader.first.stem().generic_string()) + ";\n";
        }
        return header;
    }

}

LiveReloadingShader::LiveReloadingShader(const fs::path& _shaderPath, std::vector<std::pair<fs::path, std::unique_ptr<LiveReloadingShader>>>& _shaders)
    : shaderPath(_shaderPath)
    , fw(_shaderPath.parent_path().generic_string())
    , window(std::make_unique<sf::RenderWindow>(sf::VideoMode{ 512, 512 }, _shaderPath.stem().generic_string(), sf::Style::Titlebar))
    , mousePos(.5, .5)
    , mouseEnabled(false)
    , windowClosed(false)
    , shaders(_shaders)
    , textureHeaders(GetTextureNames(shaders))
    , lastT(std::chrono::steady_clock::now())
{
    window->setVerticalSyncEnabled(true);
    window->setFramerateLimit(0);
    previousFrame.create(window->getSize().x, window->getSize().y);
    currentFrame.create(window->getSize().x, window->getSize().y);
}

LiveReloadingShader::~LiveReloadingShader() {
    window->close();
}

void LiveReloadingShader::UpdatePreviousFrame() {
    previousFrame.clear();
    sf::Sprite previousFrameSprite(currentFrame.getTexture());
    sf::RenderStates renderStates;
    renderStates.blendMode = { sf::BlendMode::One, sf::BlendMode::One };
    previousFrame.draw(previousFrameSprite, renderStates);
    previousFrame.display();
}

void LiveReloadingShader::UpdateShader() {
    bool success = false;
    do {
        success = shader.loadFromMemory(shaderHeader + textureHeaders + headerEnd + LoadFile(shaderPath.generic_string()), sf::Shader::Type::Fragment);
        if (!success) {
            std::cerr << "FAILED TO LOAD SHADER: " << shaderPath.stem().generic_string() << std::endl;
            std::this_thread::sleep_for(100ms);
        }
    } while (!success);
    std::cout << "LOADED: " << shaderPath.stem().generic_string() << std::endl;
}

void LiveReloadingShader::Tick() {
    window->clear();
    currentFrame.clear();
    const auto& size = window->getSize();
    auto now = std::chrono::steady_clock::now();
    shader.setUniform("iTime", (now - startT).count() * 1.f / std::chrono::steady_clock::period::den);
    float timeDelta = (now - lastT).count() * 1.f / std::chrono::steady_clock::period::den;
    shader.setUniform("iTimeDelta", timeDelta);
    lastT = now;
    shader.setUniform("iResolution", sf::Glsl::Vec2(size));
    shader.setUniform("iMouse", sf::Glsl::Vec2(mousePos.x * size.x, mousePos.y * size.y));

    for (const auto& otherShader : shaders) {
        const auto& name = otherShader.second->GetTextureName();
        const auto& tex = otherShader.second->GetPreviousFrameTexture();
        shader.setUniform(name, tex);
    }

    sf::Sprite currentSprite(currentFrame.getTexture());
    sf::RenderStates renderStates(&shader);
    renderStates.blendMode = { sf::BlendMode::One, sf::BlendMode::One };
    currentFrame.draw(currentSprite, renderStates);
    currentFrame.display();
    window->draw(currentSprite);

    window->display();

    sf::Event e;
    while (window->pollEvent(e)) {
        if (e.type == sf::Event::Closed) {
            windowClosed = true;
        }
        if (e.type == sf::Event::KeyReleased) {
            if (e.key.code == sf::Keyboard::Key::Escape) {
                windowClosed = true;
            }
        }
        if (e.type == sf::Event::Resized) {
            window->setView({ {e.size.width / 2.f, e.size.height / 2.f}, {1.f*e.size.width, 1.f*e.size.height} });
            currentFrame.create(e.size.width, e.size.height);
            previousFrame.create(e.size.width, e.size.height);
        }
        if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Button::Left)
            mouseEnabled = true;
        if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Button::Left)
            mouseEnabled = false;
        if (e.type == sf::Event::MouseMoved) {
            if (mouseEnabled)
                mousePos = { e.mouseMove.x * 1.f / window->getSize().x, e.mouseMove.y *  1.f / window->getSize().y };
        }
    }
}

const sf::Texture& LiveReloadingShader::GetPreviousFrameTexture() const {
    return previousFrame.getTexture();
}

std::string LiveReloadingShader::GetTextureName() const {
    return SanatizeTextureName(shaderPath.stem().generic_string());
}