
/// @author Dextron12
#ifndef WINDOW_HPP
#define WINDOW_HPP

#ifdef _WIN32
#include <Windows.h>
#endif


//OpenGL headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <functional>


namespace Dexium {
	class WindowContext {
	public:
		GLFWwindow* window;
		int width, height;

		WindowContext(const std::string& windowTitle, const int windowWidth, const int windowHeight);
		~WindowContext();

		void startFrame();
		void endFrame();

		// callbacks
		void onResize(int width, int height);
		void SetWindowResizeCallback(std::function<void()> callback);

	private:
		std::function<void()> m_resizeCallback;

	};
}


#endif