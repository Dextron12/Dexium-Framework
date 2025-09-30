
#include <core/window.hpp>
#include <core/AssetManager.hpp>

std::filesystem::path VFS::execPath;

void TraceLog(int logLevel, const char* text, ...) {
	va_list args;
	va_start(args, text);

	switch (logLevel) {
	case LOG_NONE: break;
	case LOG_TRACE: std::printf("[Trace]: "); break;
	case LOG_INFO: std::printf("[Info]: "); break;
	case LOG_WARNING: std::printf("[Warning]: "); break;
	case LOG_ERROR: std::printf("[Error]: "); break;
	case LOG_FATAL: std::printf("[FATAL]: "); break;
	default: break;
	}

	std::vprintf(text, args);
	std::printf("\n");
	va_end(args);

	if (logLevel == LOG_FATAL) {
		abort();
	}
}

WindowContext::WindowContext(const char* windowTitle, const int windowWidth, const int windowHeight) {
	if (glfwInit() == GLFW_FALSE) {
		TraceLog(LOG_FATAL, "Failed to initiate GLFW");
		return;
	}

	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
	if (!window) {
		TraceLog(LOG_FATAL, "Failed to createa  GLFWwindow");
		return;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		TraceLog(LOG_FATAL, "Failed to intiate GLAD");
		return;
	}

	// Set error callback using a lambda
	glfwSetErrorCallback([](int error, const char* description) {
		TraceLog(LOG_TRACE, "GLFW Error [%d]: %s", error, description);
	});

	VFS::init();

	//Otherwise, window was created successfully:
	glViewport(0, 0, windowWidth, windowHeight);

	glfwSetWindowUserPointer(window, this);

	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow* win, int width, int height) {
			auto* ctx = static_cast<WindowContext*>(glfwGetWindowUserPointer(win));
			if (ctx) ctx->onResize(width, height);
		}
	);

	width = windowWidth; height = windowHeight;
	appState = true;
}

WindowContext::~WindowContext() {
	glfwTerminate();
}

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

void WindowContext::endFrame() {
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void VFS::init() {
#ifdef _WIN32
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	execPath = std::filesystem::path(buffer).parent_path().parent_path();
	//execPath = std::filesystem::path(buffer).parent_path();
#endif

#ifdef _DEBUG
	// Leave debug dir:
	execPath = execPath.parent_path().parent_path(); // Should make it in project root.
#endif

}

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



std::unique_ptr<std::string> VFS::resolve(const std::string& relativePath) {
	//Check if VFS is intialized:
	if (execPath.empty()) {
		TraceLog(LOG_WARNING, "[VFS::resolve]: VFS execPath is empty. Did you forget to intialize VFS?");
		return nullptr;
	}

	std::filesystem::path rel(relativePath);
	std::filesystem::path abs = execPath / rel;

	std::filesystem::path finalPath;
	try {
		finalPath = std::filesystem::canonical(abs);
	}
	catch (const std::filesystem::filesystem_error&) {
		finalPath = abs.lexically_normal();
	}

	// Check path existance:
	if (std::filesystem::exists(finalPath)) {
		return std::make_unique<std::string>(finalPath.string());
	}
	else {
		TraceLog(LOG_WARNING, "[VFS::Resolve]: Failed to resolve path '%s'", relativePath.c_str());
		return nullptr;
	}
}

bool VFS::exists(const std::string& path) {
	if (std::filesystem::exists(path)) {
		return true;
	}
	else {
		return false;
	}
}