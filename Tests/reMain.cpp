

#include "Dexium.hpp"

#include <entt/entt.hpp>

#include "core/Transform.h"
#include "core/Texture.hpp"

class Game final : public Dexium::Core::GameLayer {
    EngineState& ctx = EngineState::get();

    Material mat = Material();

    Shader shader = Shader(VFS::resolve("Shaders/basic.vert"), VFS::resolve("Shaders/basic.frag"));
    Camera cam = Camera(Dexium::Core::ProjectionType::ORTHOGRAPHIC);
    std::unique_ptr<Dexium::Core::Mesh> mesh;
    Renderer renderer;
    Dexium::Core::Transform model = Dexium::Core::Transform();
    Dexium::Core::Texture tex;// = Dexium::Core::Texture();



    void onInit() override {
        //Init camera
        const Dexium::Core::windowContext& windowCtx = ctx.getWindowContext();
        cam.setProjection(Dexium::Core::ProjectionType::ORTHOGRAPHIC, windowCtx.width, windowCtx.height);

        // Init Mesh
        mesh = Dexium::Core::createMesh(Dexium::Core::MeshType::Mesh2D::Rectangle);
        // Compile shader
        shader.compile();
        mat.shader = &shader;

        // Transform for where the mesh should go
        Dexium::Core::Transform transform(
            glm::vec3(250,250,0.0f)
            );

        tex.flags |= Dexium::Core::TexFlags::Nearest | Dexium::Core::TexFlags::Repeat;
        tex.load("Cute_Fantasy/Trees/Big_Birch_Tree.png");

        mat.setUniform("aColor", Colour{218/255.f, 165/255.f, 32/255.f, 1.f}.rgba());
        //mat.setUniform("tex", 1);
        //shader.setUniform<glm::mat4>("projection", cam.getProjection());
        //shader.setUniform<glm::mat4>("model", transform.ModelMatrix());

        //shader.setUniform<glm::vec4>("aColor", Colour{218 / 255.f, 165 / 255.f, 32 / 255.f, 1.f}.rgba());
    }

    void onUpdate() override {
        if (glfwGetKey(ctx.getWindowContext().window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(ctx.getWindowContext().window, true);
        }
    }
    void onRender() override {
        clearColour(Color{fmt::color::cornflower_blue});

        //Dexium::Core::RenderCommand comm{mesh, }
        Dexium::Core::RenderCommand comm{&*mesh, &mat, &model };
        renderer.submit(comm);
        //tex.bind(0);
        fmt::printf(fmt::format("Texture ID: {}\n", tex.texID));


        renderer.flush();

        // The below mesh drawing is onlya fter the render::flush, because mesh isnt sent to the renderer
        // and when the renderer flushes, it clears the xcreen and renderers evertyhing ontop
        // so any external rendering must be done afterwards.

        //glBindVertexArray(mesh->VAO);

        //glDrawElements(mesh->drawMode, mesh->indexCount, GL_UNSIGNED_INT, nullptr);

    }

    void onShutdown() override {}
};

int main() {

    entt::registry idRegistry;

    entt::entity texID = idRegistry.create();

    auto& ctx = EngineState::get();

    // Override working dir
    VFS::overwriteExecPath(VFS::getExecutablePath().parent_path());

    // Configure the global logger
    Dexium::Core::GLogger.outputs |= LogOutput::Stdout | LogOutput::PrettyPrint | LogOutput::PrefixErrorType;


    ctx.attachWindow("Dexium Framework", 720, 480);
    ctx.addLayer("game", std::make_unique<Game>());
    ctx.run();

    ctx.shutdown();
}
