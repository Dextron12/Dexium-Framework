//
// Created by Dextron12 on 5/3/26.
//

#ifndef DEXIUM_SPRITE_HPP
#define DEXIUM_SPRITE_HPP

#include <core/Model.hpp>
#include <core/ResourcePool.hpp>

#include <renderer/RenderPass.hpp>

#include <variant>

namespace Dexium::Core {

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

    namespace Dexium::Core {

        std::variant<ResourceHandle<Mesh>, Mesh*> MeshVariant;
        // Generates a wuad mesh with specified params and resolve stexture from ResourceManager or directly move constructs from a temp
        class Sprite{
        public:

            //void pushDrawCmd(Renderer::RenderPass* pass,glm::vec3& pos);

            const glm::vec3& getPos();

        protected:

            Sprite() = default;




        };


        /*
// Constructs Sprite from pre-defined shader and texture
Sprite(const ResourceManager& rm, ResourceHandle<Texture> textureHandle, ResourceHandle<Shader> shaderProgram);

// Constructs Sprite from pre-defined set of textures(vec) and shader
Sprite(const ResourceManager& rm, std::vector<ResourceHandle<Texture>> textures, ResourceHandle<Shader> shaderProgram);

// Takes ownership of tempory Texture{}(Stores it in the resourceManager), shader lidfetime needs to be managed elsewhere
Sprite(const ResourceManager& rm, Texture tex, Shader* shader);

// Takes ownership of a vec of temp Tesxture{}s (Stores entire set in rm), Shader lifetime managed elsewhere
Sprite(const ResourceManager& rm, const std::vector<Texture>& textures, Shader* shader);

// Takes ownership of tempory Texture{} and handle to shader
Sprite(const ResourceManager& rm, Texture tex, ResourceHandle<Shader> shaderProgram);

Sprite(const ResourceManager& rm, const std::vector<Texture>& textures, ResourceHandle<Shader> shaderProgram);
*/

        // Construct Sprite from predefined sahder & texture
        [[nodiscard]] Sprite createSprite(const ResourceManager& rm, ResourceHandle<Shader> shaderProgram, ResourceHandle<Texture> tex);
        // Construct Sprite from pre-defined shader & vec of textures
        [[nodiscard]] Sprite createSprite(const ResourceManager& rm, ResourceHandle<Shader> shaderProgram, const std::vector<ResourceHandle<Texture>>& textures);


    }
}

#endif //DEXIUM_SPRITE_HPP