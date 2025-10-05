#include "core/Timer.hpp"

Dexium::Timer::Timer(int durationMs) : duration(durationMs), start(std::chrono::steady_clock::now()) {}

void Dexium::Timer::reset() {
	start = std::chrono::steady_clock::now();
}

bool Dexium::Timer::isFinished() const {
	return getElapsedMs() >= duration;
}

int Dexium::Timer::getElapsedMs() const {
	auto now = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
}

float Dexium::Timer::getProgress() const {
	return (float)getElapsedMs() / duration;
}

void Dexium::Timer::setDuration(int newDuration) {
	//int elapsed = getElapsedMs();
	// Maintains the same start time, changes how long the timer waits to finish.
	duration = newDuration;
}