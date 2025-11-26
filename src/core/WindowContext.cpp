
#include <core/WindowContext.hpp>
#include <core/AssetManager.hpp>

#include "Dexium.hpp"
#include "core/Error.hpp"
#include "core/helpers.hpp"
#include "core/versionControl.hpp"

#include <gl.h> // This is GLAD2(vendored)

Dexium::WindowContext::WindowContext(const std::string& windowTitle, const int windowWidth, const int windowHeight) {
	// Get Engine state
	auto& ctx = EngineState::get();

	//GLFW should already be initated
	if (!ctx.GLFW_INIT) {
		// GLFW not initalised, likely user hasnt inited the engine!!
		TraceLog(LOG_FATAL, "[WindowContext]: GLFW has not been initialised:\nDid you forget to call Dexium::init()?");
		return;
	}


	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), NULL, NULL);
	if (!window) {
		TraceLog(LOG_FATAL, "Failed to create a window");
		return;
	}
	glfwMakeContextCurrent(window);

	if (!ctx.GLAD_INIT) {
		if (!gladLoaderLoadGL()) {
			SignalManager::get().emit("GLAD_Init", false);
			return;
		} else {
			SignalManager::get().emit("GLAD_Init", true);
		}

		// set EngineState GLAD INIT FLAG
		ctx.GLAD_INIT = true;
	}



	// Configure GL viewport. For now, this is ok to do in WindowContext, but should eventually be made part of the renderer
	glViewport(0, 0, windowWidth, windowHeight); // MOVE TO CAMERA !!!!

	glfwSetWindowUserPointer(window, this);

	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow* win, int width, int height) {
			auto* ctx = static_cast<WindowContext*>(glfwGetWindowUserPointer(win));
			if (ctx) ctx->onResize(width, height);
		}
	);

	width = windowWidth; height = windowHeight;

	// initialisation complete, now enable appState
	ctx.appState = true;

	// Register the default white texture (pipeline texture)
	Dexium::Utilities::registerWhiteTexture(); // Does this belong here? No, but it works!

}

Dexium::WindowContext::~WindowContext() {
	if (!EngineState::get().ENGINE_HEADLESS) {
		glfwDestroyWindow(window);
	}
	EngineState::get().appState = false;
}

void Dexium::WindowContext::onResize(const int width, const int height) {
	this->width = width;
	this->height = height;

	// Call end-user resize hook if one is configured
	if (m_resizeCallback) {
		m_resizeCallback();
	}

	// Trigger Dexium sig_onResize
	EngineState::get().sig_windowResized.emit(width, height);

}

void Dexium::WindowContext::SetWindowResizeCallback(std::function<void()> callback) {
	m_resizeCallback = std::move(callback);
}

void Dexium::WindowContext::startFrame() {
	if (glfwWindowShouldClose(window)) {
		// Set appState to false, to shutdown the engine
		EngineState::get().appState = false;
		EngineState::get().shutdown();
	}
}

/* DEPRECTATED FUNCTION --> deltaTime should be moved to EngineState or Layer system.
void WindowContext::startFrame() {
	//Update deltaTime and FPSTimer
	double currentTime = glfwGetTime();
	deltaTime = (currentTime - lastTime) * 1000.0f; //Make sure its in Ms.
	lastTime = currentTime;

	//FPS logic
	frameCount++;
	fpsTimer += deltaTime;
	if (fpsTimer >= 1.0) {
		FPS = frameCount;
		frameCount = 0;
		fpsTimer = 0.0f;
	}

	if (glfwWindowShouldClose(window)) appState = false;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) appState = false;
}
*/ // End of DEPRECATED BLOCK

void Dexium::WindowContext::endFrame() {
	glfwPollEvents();
	glfwSwapBuffers(window);
}



/* DEPRECATED WindowContext FUUNCTION --> NO LONGER NEEDED -> KEPT IN CASE I FORGOT SOMEHTING

bool WindowContext::toggleWireFrame() {
	drawWireframe = !drawWireframe;
	glPolygonMode(GL_FRONT_AND_BACK, drawWireframe ? GL_LINE : GL_FILL);
	TraceLog(LOG_INFO, "[WindowContext] WireFrame mode: %s", (drawWireframe ? "enabled" : "disabled"));
	return drawWireframe;
}

void WindowContext::onResize(int width, int height) {
	glViewport(0, 0, width, height);
	this->width = width; this->height = height;

	// Call end-users custom resizeHook if one is provided
	if (m_resizeCallbackScript) {
		m_resizeCallbackScript();
	}

	TraceLog(LOG_INFO, "Window FrameBuffer has resized");
}


// Window lambda setters
void WindowContext::setWindowResizeCallback(std::function<void()> callback) {
	m_resizeCallbackScript = std::move(callback);
}

*/


