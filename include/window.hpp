
#ifndef WINDOW_HPP
#define WINDOW_HPP

#ifdef _WIN32
#include <Windows.h>
#endif


//OpenGL headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <filesystem>
#include <cstdarg> // For va_list & va_start

#define LOG_TRACE	1
#define LOG_DEBUG	2
#define LOG_INFO	3
#define LOG_WARNING 4
#define LOG_ERROR	5
#define LOG_FATAL	6
#define LOG_NONE	0

void TraceLog(int logLevel, const char* text, ...);

class WindowContext {
public:
	GLFWwindow* window;
	int width, height;
	float deltaTime = 0.0f;
	int FPS = 0;
	

	//Window state var:
	bool appState = false;

	WindowContext(const char* windowTitle, const int windowWidth, const int windowHeight);
	~WindowContext();

	void startFrame();
	void endFrame();

private:
	double lastTime = glfwGetTime();
	int frameCount;
	double fpsTimer = 0.0f;
};

class VFS {
public:
	static void init();

	static std::unique_ptr<std::string> resolve(const std::string& relativePath);

	static bool exists(const std::string& path);

	static std::filesystem::path execPath;
};

#endif