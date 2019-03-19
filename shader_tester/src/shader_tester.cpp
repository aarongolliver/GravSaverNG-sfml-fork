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
    const auto& projectDirectory = args[1];

    std::vector<std::pair<fs::path, std::unique_ptr<LiveReloadingShader>>> shaders;
    for (const auto & shaderFile : fs::directory_iterator(projectDirectory)) {
        shaders.emplace_back(std::make_pair(shaderFile.path(), nullptr));
    }

    for (auto& shader : shaders) {
        shader.second = std::make_unique<LiveReloadingShader>(shader.first, shaders);
    }

    for (const auto& shader : shaders) {
        shader.second->UpdateShader();
    }

    SimpleFileWatcher fw(projectDirectory);
    int i = 0;
    const int t = 100;
    using namespace std::chrono;
    time_point<steady_clock> tSamplesAgo = steady_clock::now();
    while (1) {
        if (++i % t == 0) {
            i = 0;
            auto now = steady_clock::now();
            std::cout << 1. / ((now - tSamplesAgo).count() * (1. / steady_clock::period::den) / (1. * t)) << std::endl;
            tSamplesAgo = now;
        }
        for (const auto& shader : shaders) {
            shader.second->UpdatePreviousFrame();
        }

        if (fw.CheckChanged()) {
            {
                std::vector<fs::path> currentFileset;
                for (const auto & shaderFile : fs::directory_iterator(projectDirectory)) {
                    currentFileset.emplace_back(shaderFile.path());
                }

                // see if any of the currently loaded shaders are no longer in the folder
                // and remove them
                for (auto shaderItr = shaders.begin(); shaderItr != shaders.end(); ) {
                    bool found = false;
                    for (const auto& currentFile : currentFileset) {
                        if (currentFile.generic_string() == shaderItr->first.generic_string())
                            found = true;
                    }
                    if (!found)
                        shaderItr = shaders.erase(shaderItr);
                    else
                        ++shaderItr;
                }

                // see if any of the current files are not loaded as shaders, and add them
                for (const auto& currentFile : currentFileset) {
                    bool found = false;
                    for (const auto& currentShader : shaders) {

                        if (currentFile.generic_string() == currentShader.first.generic_string())
                            found = true;
                    }
                    if (!found)
                        shaders.emplace_back(std::make_pair(currentFile, std::make_unique<LiveReloadingShader>(currentFile, shaders)));
                }
            }

            for (const auto& shader : shaders) {
                shader.second->UpdatePreviousFrame();
            }

            for (const auto& shader : shaders) {
                shader.second->UpdateShader();
            }
        }

        for (const auto& shader : shaders) {
            shader.second->Tick();
            if (shader.second->windowClosed)
                return;
        }

        for (const auto& shader : shaders) {
            if (shader.second->gainedFocus)
            {
                for (const auto& shader : shaders) {
                    shader.second->RequestFocus();
                }
                for (const auto& shader : shaders) {
                    shader.second->gainedFocus = false;
                }
                break;
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