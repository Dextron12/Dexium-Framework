//
// Created by Dextron12 on 14/09/2025.
//

#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP

#include <memory>


#include <core/Shader.hpp>
#include <core/Mesh.hpp>

namespace Dexium {
    class Primitives {
    public:
        static void FilledRect(glm::vec4 pos, glm::vec4 colour, const Shader& shader);
        static void Rect(glm::vec4 pos, glm::vec4 colour, const Shader& shader, int thickness = 1);

        static void Line(glm::vec2 startPos, glm::vec2 endPos, glm::vec4 colour, const Shader& shader);

        // Should eventually add Circle(anit-aliased?) support

        static std::shared_ptr<Mesh>& getUnitRect() {
            static auto mesh = MeshFactory::QuadWithUV();
            return mesh;
        }

    private:
    };


}

#endif //PRIMITIVES_HPP