
/// @author Dextron12
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
#include <functional>

/// @defgroup DexiumLog Dexium Error Codes
/// @brief Logging Levels for Dexium engine messages
/// @{
#define LOG_TRACE    1 ///< TRACE: debug or tracking messages
#define LOG_DEBUG    2 ///< DEBUG: messages output only in debug builds
#define LOG_INFO     3 ///< INFO: general engine state information
#define LOG_WARNING  4 ///< WARNING: minor failure
#define LOG_ERROR    5 ///< ERROR: major failure, affected data likely lost
#define LOG_FATAL    6 ///< FATAL-ERROR: resource could not load, application may crash
#define LOG_NONE     0 ///< Disable all logging
/// @}

void TraceLog(int errorLevel, const char* format, ...);

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

	// Toggles:
	bool toggleWireFrame();

	// Callbacks:
	void onResize(int width, int height);
	void setWindowResizeCallback(std::function<void()> callback);

private:
	double lastTime = glfwGetTime();
	int frameCount;
	double fpsTimer = 0.0f;

	// Callback layers (Lambda scripting hooks)
	std::function<void()> m_resizeCallbackScript;

	// GL optional states:
	bool drawWireframe = false; 
};

class VFS {
public:
	static void init();

	static std::unique_ptr<std::string> resolve(const std::string& relativePath);

	static bool exists(const std::string& path);

	static std::filesystem::path execPath;
};

#endif