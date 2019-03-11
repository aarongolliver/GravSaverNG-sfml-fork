#pragma once

#include <string>
#include <filesystem>
#include <memory>
#include "file_watcher.h"
#include <SFML/Graphics.hpp>

namespace sf { class RenderWindow; }

#ifdef _EXPERIMENTAL_FILESYSTEM_
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

class LiveReloadingShader {
public:
    LiveReloadingShader(const fs::path& path);
    ~LiveReloadingShader();

    void Tick();

private:
    const fs::path shaderPath;
    const std::unique_ptr<sf::RenderWindow> window;
    SimpleFileWatcher fw;

    sf::Shader shader;
    sf::RenderTexture texture;
    sf::Vector2f mousePos;

    bool mouseEnabled;

public:
    bool windowClosed;
};