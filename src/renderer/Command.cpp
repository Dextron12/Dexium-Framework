//
// Created by ethan on 30/3/26.
//

#include <renderer/Command.hpp>

Dexium::Renderer::Command::Command(Core::Mesh* mesh, Core::Material* material, Core::Transform* transform)
    : mesh(mesh), material(material), transform(transform) {}
