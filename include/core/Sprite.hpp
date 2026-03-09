//
// Created by Dextron12 on 5/3/26.
//

#ifndef DEXIUM_SPRITE_HPP
#define DEXIUM_SPRITE_HPP

#include <core/VFS.hpp>

#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <renderer/RenderTarget.hpp>
#include <core/Texture.hpp>

namespace Dexium::Core {
    //Forward declares
    class Material;
    class Mesh;
    class Transform;
    class Renderer;

    /*
     *In my infinite escape to avoid an AssetManager(This very engine used to have one, before it burnt me)
     * Its important that some basic ground rules are in place:
     * 1. Renderables 9Sprites, etc) are observers of resources.
     *    They NEVER own rendering resources
     *
     * 2. Consumables (Texture, Mesh, Material, Shader) own and manage
     *    their own lifetime independently.
     *
     * 3. Users must be able to directly construct consumables:
     *    Texture tex("player.png")
     *
     * 4. A ResourcePool MAY exist to create, cache adn track
     *    resources for convience
     *
     * 5. Renderables & Consumables msut never depend on
     *    the ResourcePool or asset system
     *
     *  The Asset system is OPTIONAL infrastructure, not a core dependency
     */

    class Sprite {
    public:
        Sprite() = default;
        //Sprite is a renderable(Non-owning manager) of lower level resoruces

        Texture* transform = nullptr;
        Mesh* mesh = nullptr;
        Material* material = nullptr;

        RenderState::RenderTarget* rendrTgt = nullptr; // Non-owning, by default uses window-wide renderTarget

        glm::vec3 position; // Pos (in screen space) of where to render Sprite
        glm::vec2 scaleMultiplier; // Provides multiplicative scaling of texture. (Texture scaling is internally done regardless)
        float rotation; // How many degrees to rotate the Sprite (+: Clockwise Rot, -: Anticlockwise Rot)


        void load(const std::filesystem::path& texturePath, TexFlags imgFlags);
    private:
        std::unique_ptr<Transform> transform;



    };
}

#endif //DEXIUM_SPRITE_HPP