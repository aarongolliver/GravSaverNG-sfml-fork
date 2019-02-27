#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <SFML/Graphics.hpp>

#include "FileWatcher/FileWatcher.h"

template<typename T>
class LambdaWatcher : public FW::FileWatchListener {
    const T _func;
public:
    LambdaWatcher(T& func) : _func(func) {
    }
    ~LambdaWatcher() override = default;
    void handleFileAction(FW::WatchID, const FW::String&, const FW::String&, FW::Action) override {
        _func();
    }
};

void _main(std::vector<std::string> args) {
    FW::FileWatcher fw;
    auto f = [&]() {
        std::cout << "CHANGE" << std::endl;
    };
    LambdaWatcher<decltype(f)> watcher(f);
    fw.addWatch(args[1], &watcher);
    sf::RenderWindow window({ 512, 512, 32 }, "shader toy", sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(0);

    while (window.isOpen()) {
        window.clear();

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

}










int main(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    _main(std::move(args));

    return EXIT_SUCCESS;
}