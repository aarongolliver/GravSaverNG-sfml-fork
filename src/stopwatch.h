#pragma once

namespace  {
    const auto STOPWATCH_DEBUG = false;
}

class StopWatch
{
public:
	StopWatch(const std::string& message) : message(message) {
        if(STOPWATCH_DEBUG)
		    start = std::chrono::high_resolution_clock::now();
	};

	void Tick() {
        if (!STOPWATCH_DEBUG) return;
		auto now = std::chrono::high_resolution_clock::now();
		auto delta = now - start;
		auto seconds = delta.count() / 1e9f;
		auto fps = (1. / seconds);

		start = now;

		std::cout << message << ", " << fps << std::endl;
	}
	~StopWatch()
	{
		Tick();
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	const std::string message;
};
