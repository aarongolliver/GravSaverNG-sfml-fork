#include "shader_tester.h"

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <filesystem>
#include <unordered_map>

#include "live_reloading_shader.h"

#ifdef _EXPERIMENTAL_FILESYSTEM_
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

using namespace std::chrono_literals;

void _main(std::vector<std::string> args) {
    std::unordered_map<std::string, std::unique_ptr<LiveReloadingShader>> shaders;
    for (const auto & shaderFile : fs::directory_iterator(args[1])) {
        shaders.emplace(shaderFile.path().generic_string(), std::make_unique<LiveReloadingShader>(shaderFile.path()));
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