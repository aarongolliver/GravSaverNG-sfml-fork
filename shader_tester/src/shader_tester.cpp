#include "shader_tester.h"

#include "shader_header.h"

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <filesystem>
#include <tuple>
#include <thread>
#include <chrono>

#if !_HAS_CXX17
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

using namespace std::chrono_literals;

void _main(std::vector<std::string> args) {
    std::vector<std::pair<fs::path, std::unique_ptr<LiveReloadingShader>>> shaders;
    for (const auto & shaderFile : fs::directory_iterator(args[1])) {
        shaders.emplace_back(std::make_pair(shaderFile.path(), nullptr));
    }

    for (auto& shader : shaders) {
        shader.second = std::make_unique<LiveReloadingShader>(shader.first, shaders);
    }

    while (1) {
        for (const auto& shader : shaders) {
            shader.second->Tick();
            if (shader.second->windowClosed)
                return;
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