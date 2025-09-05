#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "Material.hpp"
#include "Camera.hpp"

class Sprite {
public:
	Sprite(const std::string& spritePath, std::shared_ptr<Camera> camera);
	Sprite(std::shared_ptr<Material> material, const std::string& spritePath, std::shared_ptr<Camera> camera);

	 const std::shared_ptr<Material>& getMaterial();

	 /* Because of some frustations - it's important to note that Material is bound once every render() call*/
	 void render(const Transform& transform);

	 // Scale Sprite by w & h offset (x, y, w, h)
	 void render(const glm::vec4& pos);

protected:
	Transform model; // The pos, scale and rot of sprite. DO NOT manually set scale, instead use setSize(). En

	Renderable m_renderable;

	virtual void update();

private:
	std::shared_ptr<Camera> m_camera;
	
};


/*
class Spritesheet {
public:
	Spritesheet(const std::string& spritePath, std::shared_ptr<Camera> camera);
	Spritesheet(const std::string& spritePath, const std::string& shaderID, std::shared_ptr<Camera> camera);

private:
	std::string m_shaderID;


};

*/

#endif