#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <memory>

#include "Material.hpp"
#include "Camera.hpp"


struct SpriteInstance {
	Transform model;		// world transformation
	int frameIndex;			// which subtexture to use
};

// Subtexture metadata stored in SSBO
struct SubTexture {
	glm::vec2 uvMin;
	glm::vec2 uvMax;
};

class Sprite {
public:
	Sprite(const std::string& spritePath, std::shared_ptr<Camera> camera);
	Sprite(std::shared_ptr<Material> material, const std::string& spritePath, std::shared_ptr<Camera> camera);

	 const std::shared_ptr<Material>& getMaterial();

	 /* Because of some frustrations - it's important to note that Material is bound once every render() call*/
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


class Spritesheet {
public:
	Spritesheet(const std::string& fileName);
	~Spritesheet();

	void setSprite(const std::string& spriteID, const glm::vec4& spriteDim);
	void remSprite(const std::string& spriteID);

	// Configures the Spritesheet to use a specified shader program (Through AssetManager)
	void setShader(const std::string& shaderID);

	// Loads the internal spritesheet & congifure properties
	void load(const std::string& fileName = "");

	void drawSprite(const std::string& spriteID, const glm::vec3 pos, glm::vec3 scale = glm::vec3(1.0), glm::vec3 rot = glm::vec3(0.0));

	std::shared_ptr<Camera> camera;
private:
	std::unique_ptr<Renderable> m_sprite; // The Renderable context
	std::string m_spriteID; // The internal AssetManager ID for the Texture2D ptr

	std::string m_shaderID;

	std::unordered_map<std::string, glm::vec4> m_frames; // ID --> (u0,v0,u1,v1) -> (x,y,w,h)
};

#endif