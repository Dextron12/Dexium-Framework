//
// Created by ethan on 1/4/26.
//

#ifndef DEXIUM_MODEL_HPP
#define DEXIUM_MODEL_HPP

#include <memory>

#include <core/Mesh.hpp>
#include <core/Material.hpp>
#include <core/Transform.h>

namespace Dexium::Core {

    class Model {
    public:
        Model() = default; // SHould ONLY be used for type-specification. Use param ctor for construction(or jsut sue a handle>

        Model(Mesh mesh, Material material, Transform transform)
            : m_Mesh(std::make_unique<Mesh>(std::move(mesh))),
        m_Material(std::make_unique<Material>(std::move(material))),
        m_Transform(std::make_unique<Transform>(std::move(transform))) {}

        std::unique_ptr<Mesh> m_Mesh = nullptr;
        std::unique_ptr<Material> m_Material = nullptr;
        std::unique_ptr<Transform> m_Transform = nullptr;

    };
}

#endif //DEXIUM_MODEL_HPP