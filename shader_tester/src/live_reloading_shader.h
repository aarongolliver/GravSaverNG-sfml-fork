#pragma once

#include <string>
#include <filesystem>
#include <memory>
#include "file_watcher.h"
#include <vector>
#include <SFML/Graphics.hpp>

namespace sf { class RenderWindow; }

#if !_HAS_CXX17
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

class LiveReloadingShader {
public:
    LiveReloadingShader(const fs::path& path, std::vector<std::pair<fs::path, std::unique_ptr<LiveReloadingShader>>>& shaders);
    ~LiveReloadingShader();

    std::string GetTextureName() const;
    const sf::Texture& GetPreviousFrameTexture() const;
    void UpdatePreviousFrame();
    void UpdateShader();

    void Tick();

private:
    const fs::path shaderPath;
    const std::unique_ptr<sf::RenderWindow> window;
    SimpleFileWatcher fw;

    sf::Shader shader;
    sf::RenderTexture currentFrame;
    sf::RenderTexture previousFrame;
    sf::Vector2f mousePos;

    bool mouseEnabled;

    std::vector<std::pair<fs::path, std::unique_ptr<LiveReloadingShader>>>& shaders;
    const std::string textureHeaders;

public:
    bool windowClosed;
};