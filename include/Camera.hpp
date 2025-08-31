#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

enum ProjectionType {
	PERSPECTIVE,
	ORTHOGRAPHIC,
	UNDEFINED
};

class Camera {
public:
	// Setting the projType to Orthographic, internally fixes it as a 2D camera. Setting it perspective sets it as a 3D camera.
	Camera(int screenWidth, int screenHeight, ProjectionType projType);

	const glm::mat4& getProjection() const;

	ProjectionType type;

private:
	glm::mat4 projection;

	glm::vec2 windowSize;
};

#endif
