#include "Camera.hpp"


Camera::Camera(int screenWidth, int screenHeight, ProjectionType projType) : windowSize(screenWidth, screenHeight), type(projType) {
	// Configure projection
	if (type == ORTHOGRAPHIC) {
		// Assumes a 2D projection (render in screen space(px))
		projection = glm::ortho(0.0f, (float)windowSize.x, (float)windowSize.y, 0.0f);
		/**
		* This projection breaks OpenGL convention - OpenGL expects (0,0) to be bottom left of screen. 
		This sets it to top left.( As expected in a 2D renderer)
		*/
	}
	else if (type == PERSPECTIVE) {
		// Standard 3D projection
		projection = glm::perspective(glm::radians(45.0f), (float)windowSize.x / (float)windowSize.y, 0.1f, 100.0f);
	}


	// Eventually thjis should be configured so an end-user can easily set FOV, near and far planes
}

const glm::mat4& Camera::getProjection() const {
	return projection;
}