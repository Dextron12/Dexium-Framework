//
// Created by ethan on 5/12/25.
//

#ifndef DEXIUM_MESH_H
#define DEXIUM_MESH_H

#include <memory>
#include <variant>
#include <functional>

#include <glad/gl.h>

namespace Dexium::Core {

    namespace MeshType {
        enum class Mesh2D {
            Triangle,
            Rectangle,
            Circle,
            Line
        };

        enum class Mesh3D {
            Cube,
            Sphere,
            Cylinder,
            Plane
        };
    }

    class Mesh {
    public:
        unsigned int VBO = 0; // (Vertex Buffer Objext) A buffer that holds info on positions etc
        unsigned int VAO = 0; // (Vertex Attribute State tracker) essentially caches any buffer bound to is. So this can be bound to re-bind the VBO and EBO (if in use)
        unsigned int EBO = 0; // (Element Buffer Object) Allows user to set indices and reduce vertex counts

        int vertexCount = 0; int indexCount = 0;
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        // Not the clearest API
        using _MeshType = std::variant<MeshType::Mesh2D, MeshType::Mesh3D>;

        GLenum drawMode = GL_TRIANGLES;
        GLenum usageHint = GL_STATIC_DRAW;

        Mesh() = default; // default constucts a mesh (Should onlyu be used for type specification purposes)
        ~Mesh() {destroy(); }

        // Tranform is no longer a part of Mesh, It would be its own entity
        // Rendering a Mesh directly is no longer possible, unless you built its own entity that commands the renderer how to do so
        // Mesh no loinger uploads its own data!! Instead the createMesh function will do this. (Or a function that creates a custom mesh, will do this too)

    private:
        void destroy(); // Safely destroys and frees the GL buffers

        bool usingEBO() const { return EBO != 0; } // Helper to determine if theres an active EBO

        // Private func thast generates the meshes
        void generateMesh(MeshType::Mesh2D type);
        void generateMesh(MeshType::Mesh3D type);

        // Relations
        friend std::unique_ptr<Mesh> createMesh(Mesh::_MeshType, const std::function<void()>& setupAttribs);

    };
    // Your one stop shop to generating mesh's in Dexium. This helper function has two overloads:
    // 1. Generates a pre-defined mesh and optionally allows overrding the default attrib locations
    // 2. Generates a custom mesh defined from provided vertex/indice information and optionalally overridden default attrib locations
    // NOTE: by providing a lambda to setupAttribs, you must bind and enable the attribs yourself.
    std::unique_ptr<Mesh> createMesh(Mesh::_MeshType, const std::function<void()>& setupAttribs = nullptr);
    std::unique_ptr<Mesh> createMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indicies, std::function<void()> setupAttribs);


}

namespace Dexium::MeshData {
        inline std::vector<float> quadVertices {
            // x,     y,       z,        u,    v
            0.5f,   0.5f,   0.0f,       1.0f, 1.0f,        // top right
            0.5f,   -0.5f,  0.0f,       1.0f, 0.0f,       // bottom right
            -0.5f,  -0.5f,  0.0f,       0.0f, 0.0f,
            -0.5f,  0.5f,   0.0f,       0.0f, 1.0f

        };

        inline std::vector<unsigned int> quadIndices = {
            0, 1, 3,     // first triangle
            1, 2, 3  // second triangle
        };

    inline std::vector<unsigned int> triIndices = {
        0, 1, 2
    };
}


#endif //DEXIUM_MESH_H