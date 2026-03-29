//
// Created by ethan on 29/3/26.
//

#ifndef DEXIUM_COMMAND_HPP
#define DEXIUM_COMMAND_HPP

//Forward declares
namespace Dexium::Core {
    class Mesh;
    class Material;
    class Transform;
}

namespace Dexium::Renderer {

    class Command {
    public:
        Core::Mesh* mesh;
        Core::Material* material;
        Core::Transform* transform;

        Command(Core::Mesh* mesh, Core::Material* material, Core::Transform* transform);
    };

}

#endif //DEXIUM_COMMAND_HPP