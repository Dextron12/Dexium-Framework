//
// Created by ethan on 5/12/25.
//

#include <core/Mesh.hpp>

#include <core/Error.hpp>

namespace Dexium::Core {

    void Mesh::destroy() {
        // I think because Mesh is managed by a unique_ptr, calliung glDelete on the buffers is a double delete -> SEGFAULT
        //if (VBO) glDeleteBuffers(1, &VBO);
        //if (VAO) glDeleteVertexArrays(1, &VAO);
        //if (EBO) glDeleteBuffers(1, &EBO);

        // 0 assign ID's, so stale meshes dont accidentally use other buffer ID's
        VBO = 0;
        VAO = 0;
        EBO = 0;
    }

    // Generate Mesh (2D Variant)
    void Mesh::generateMesh(MeshType::Mesh2D type) {
        switch (type) {
            case MeshType::Mesh2D::Triangle:
                vertices = MeshData::quadVertices;
                indices = MeshData::triIndices;
                break;
            case MeshType::Mesh2D::Rectangle:
                vertices = MeshData::quadVertices;
                indices = MeshData::quadIndices;
                break;
            case MeshType::Mesh2D::UDEF:
                // User is providng their own vertices/indices
                break;
            default:
                TraceLog(LogLevel::WARNING, "[Mesh]: Unsupported mesh type requested to generate");
                break;
        }

        // Calculate the VertexCount + IndiceCount from the containers
        vertexCount = static_cast<int>(vertices.size() / 5);
        indexCount = static_cast<int>(indices.size());
    }

    void Mesh::generateMesh(MeshType::Mesh3D type) {
        TraceLog(LogLevel::WARNING, "[Mesh]: Hold ya horsies!! Dexium is still in experimental 2D and therefore no 3D meshes exist yet!");
    }

    void Mesh::buildMesh(const std::function<void()>& setupAttribs) {
        if (vertices.size() == 0) {
            TraceLog(LogLevel::ERROR, "[Mesh]: No vertices provided on custom profile");
            return;
        }
        if (vertexCount == 0) {
            TraceLog(LogLevel::ERROR, "[Mesh]: vertices provided, but no count on custom profile");
            return;
        }
        //Can still build a mesh without indices, just dont sue EBO

        // Generate & bind VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Generate & bind VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // Uplaod buffer data
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), usageHint);

        // Generate an EBO if provided
        if (indices.size() > 0) {
            glGenBuffers(1, &EBO);
        }

        // Uplaod EBO
        if (usingEBO()) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices.data(), usageHint);
        }

        // WARNING: THIS FN IS ALSO USED FOR CUSTOM MESHES, CANNOT OUTPUT A NY LOGS THOUGH AS THE PRE-GENERATED MESHES ALSO USE THIS FN
        // SO IT IS ENTIRELY UP TO THE END-USER TO ENSURE THEY ARE CORRECTLY CONFIGURING THESE ATTRIBS
        // IF NO ATTRIBS (setupAttribs) ARE PROVIDED, WE WILL USE THE DEFAULT:
        // Position (vert 3) attrib 0
        // UVs (vert 2) attrib 1
        // In this default case, vertice data should be 5 comps long per vertex. So for a single triangle under this default, the vertices should be 3*5=15 in size

        if (setupAttribs) {
            setupAttribs();
        } else {
            // Use defaults
            // Pos (0-3)
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // UVs (4-5)
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }

        // Unbind buffers to prevent unintended editing
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }



    // The creme de la creme
    std::unique_ptr<Mesh> createMesh(Mesh::_MeshType type, const std::function<void()>& setupAttribs) {
        // New Mesh instance
        auto mesh = std::make_unique<Mesh>();

        // Generate CPU side Mesh data
        if (std::holds_alternative<MeshType::Mesh2D>(type)) {
            // Generate 2D Mesh
            auto value = std::get<MeshType::Mesh2D>(type); // Actual enum value
            mesh->generateMesh(value); // generateMesh is private func, uses this to direclty modify the mesh
        } else if (std::holds_alternative<MeshType::Mesh3D>(type)) {
            // Generate 3D Mesh
            auto value = std::get<MeshType::Mesh3D>(type);
            mesh->generateMesh(value);
        }

        // No validation of Meshdata needeed, as these are internal engine functions

        // Generaye &  bind VAO
        glGenVertexArrays(1, &mesh->VAO);
        glBindVertexArray(mesh->VAO);

        // Generate + bind VBO
        glGenBuffers(1, &mesh->VBO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);

        // Upload buffer data
        glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(float), mesh->vertices.data(), mesh->usageHint);

        // Generate an EBO if indice data exists
        if (mesh->indexCount > 0) {
            glGenBuffers(1, &mesh->EBO);
        }

        // Upload EBO
        if (mesh->usingEBO()) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indexCount * sizeof(unsigned int), mesh->indices.data(), mesh->usageHint);
        }

        // Bind Vertex data(pos) to attrib 0
        // Bind UV data to          attrib 1
        // Unless overridden
        if (setupAttribs) {
            setupAttribs();
        } else {
            // Vertex data (Based off the layout of the default 2D shader)
            // x, y, z, u, v
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // UV data
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }

        // Unbind buffers (so they are not accidentally modified by state)
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbinds VBO (Does not unbind the internal VAO from the buffer)
        glBindVertexArray(0); // Unbinds the VAO

        // Return the generated mesh
        return mesh;
    }
}