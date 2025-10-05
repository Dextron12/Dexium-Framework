#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <memory>

#include "Material.hpp"
#include "Camera.hpp"

namespace Dexium {

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
		Sprite(const std::string& fileName);
		~Sprite();

		// Performs validation checks against AssetManager(The holy monolith of this project)
		void setShader(const std::string& shaderID);

		// Vec4(x,y, w,h) Leave w&h = 0 if you want to use defualt texture size
		void drawSprite(const glm::vec4 pos, const glm::vec3 rot = glm::vec3(0.0f));

		std::shared_ptr<Camera> camera;

	private:
		std::unique_ptr<Renderable> m_sprite; // The renderable context
		std::string m_spriteID; // The internal AssetManager ID for the Texture2D

		std::string m_shaderID;


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

		const std::unordered_map<std::string, glm::vec4>& getSpriteFrames() { return m_frames; }

		const std::string& getTextureID();

		std::shared_ptr<Camera> camera;
	private:
		std::unique_ptr<Renderable> m_sprite; // The Renderable context
		std::string m_spriteID; // The internal AssetManager ID for the Texture2D ptr

		std::string m_shaderID;

		std::unordered_map<std::string, glm::vec4> m_frames; // ID --> (u0,v0,u1,v1) -> (x,y,w,h)
	};


}

#endif