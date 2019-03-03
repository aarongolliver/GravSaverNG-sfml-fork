#pragma once

#include <FileWatcher/FileWatcher.h>
#include <windows.h>

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

void clearcmd() {
    COORD topLeft = { 0, 0 };
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(console, &screen);
    FillConsoleOutputCharacterA(
        console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    FillConsoleOutputAttribute(
        console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    SetConsoleCursorPosition(console, topLeft);
}