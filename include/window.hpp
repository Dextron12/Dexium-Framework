
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


/// @ingroup DexiumLog
/// @brief Outputs a log message following Dexium engine standards
/// 
/// Prints a message to the engine log witht he given log level.
/// Supported log levels are defined in the Dexium log codes (LOG_TRACE, LOG_DEBUG, etc).
/// 
/// @param logLevel The severity of the message (use Dexium log codes)
/// @param text The message format string (printf-style)
/// @param ... Optional additonal arguments for the format string
void TraceLog(int logLevel, const char* text, ...);


/**
* @defgroup Window
* @class WindowContext
* @brief The Dexium Window Manager class.
* 
* Uses GLFW as the windwo backend. Provides window management,
* timing (delta time), and frame handling(HIGHLY recommended if using Dexium systems).
* Supports toggles (wireframe mode) and callback handling.
*/
class WindowContext {
public:
	GLFWwindow* window;
	int width, height;
	float deltaTime = 0.0f;
	int FPS = 0;
	

	//Window state var:
	bool appState = false;

	/**
	* @ingroup Window
	* @brief Construct a new WindowContext.
	* 
	* @param windowTitle Title of your application
	* @param windowWidth Initial window width (in pixels)
	* @param windowHeight Initial window height (in pixels)
	*/
	WindowContext(const char* windowTitle, const int windowWidth, const int windowHeight);
	~WindowContext();

	/**
	* @ingroup Window
	* @brief Marks the beginning of a frame
	* @details Updates delta time and handles window management.
	*/
	void startFrame();

	/**
	* @ingroup Window
	* @brief Marks the end of a frame.
	* @details Swaps buffers & polls events
	*/
	void endFrame();

	// Toggles:
	/**
	* @ingroup Window
	* @brief Toggles the wireframe rendering mode.
	* @return True if wireframe mode is enabled, false if disabled
	*/
	bool toggleWireFrame();

	// Callbacks:
	/**
	* @ingroup Window
	* @brief handles window resize events.
	* 
	* @param width New window width.
	* @param height New window Height.
	*/
	void onResize(int width, int height);

private:
	double lastTime = glfwGetTime();
	int frameCount;
	double fpsTimer = 0.0f;

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