//
// Created by Dextron12 on 22/12/25.
//

// This si a TEST of Dexium (0.16.2) to render textures with the new renderer

#include <Dexium.hpp>

#include "core/Texture.hpp"
#include "utils/ID.hpp"

#include <entt/signal/sigh.hpp>

std::vector<float> vertices = {
    // positions      // texCoords
    -0.5f, -0.5f, 0.f,  0.f, 0.f,
     0.5f, -0.5f, 0.f,  1.f, 0.f,
     0.5f,  0.5f, 0.f,  1.f, 1.f,
    -0.5f,  0.5f, 0.f,  0.f, 1.f
};

std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

auto resizeListener = [](int w, int h) {
    TraceLog(ErrorType::STATUS, "Window resized to W: ({}), H: ({})", w, h);
};


class Game : public Dexium::Core::GameLayer {
public:

    EngineState& ctx;
    //Renderer renderer;

    //Game() : ctx(EngineState::get()), renderer(&ctx.getWindowContext()) {
    //}

    //Shader shader;
    Dexium::Core::Mesh mesh;

    Dexium::Core::Texture tex;

    Dexium::Utils::UUID id;
    Dexium::Utils::IDHandle id1;

    entt::sigh<void(int, int)> sig_onWindowResize;

    void onInit() override {
        //Allow Alpha-rendering (Should think opf some clean abstraction for this) Perhaps make a ALPHA flag for GWinMasks and then engine can handle it
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        clearColour(fmt::color::cornflower_blue);

        //shader = Shader("Shaders/basic.vert", "Shaders/basic.frag");
        //shader.compile();
        //After shader compialtion, set uniforms
        //shader.bind(); // Purely to set uniforms, not needed if setting through a amterial(as they are internally stored and set by the renderer)
        //auto col = Colour::fromBytes(218 , 165, 32, 255.f);
        //shader.setUniform<glm::vec4>("aColor", col.rgba());


        mesh = *Dexium::Core::createMesh(Dexium::Core::MeshType::Mesh2D::Rectangle); //dodgy asf
        mesh.vertices = vertices;
        mesh.indices = indices;

        GLenum err = glGetError();
        tex.flags |= Dexium::Core::TexFlags::Nearest | Dexium::Core::TexFlags::Repeat | Dexium::Core::TexFlags::Mipmaps;
        tex.load("/Cute_Fantasy/Buildings/Buildings/Houses/Limestone/House_2_Limestone_Base_Black.png");

        if (err != GL_NO_ERROR) {
            TraceLog(ErrorType::ERROR, "GL Error: {}", err);
        }

        TraceLog(ErrorType::DEBUG, "[ID]: {}", id.str());
        id = Dexium::Utils::UUID::Generate();
        TraceLog(ErrorType::DEBUG, "[ID]: {}", id.str());

        id1.setLabel("House");
        TraceLog(ErrorType::FATAL, "[IDHandle]: {}", id1.str());

        entt::sink{sig_onWindowResize}.connect<&decltype(resizeListener)::operator()>(resizeListener);

        entt::sink{Dexium::Globals::Signals::sig_onWindowResize}.connect<&decltype(resizeListener)::operator()>(resizeListener);
    }

    void onUpdate() override {

        if (glfwGetKey(ctx.getWindowContext().window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(ctx.getWindowContext().window, true);
        }
    }

    void onRender() override {


        renderer.flush();


        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            TraceLog(ErrorType::ERROR, "GL Error (before rendering): {}", err);
        }

        //shader.bind();
        tex.bind(0);
        //shader.setUniform("uTexture", 0);
        glBindVertexArray(mesh.VAO);

        if (mesh.EBO) {
            //Element-wise drawing
            glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
        } else {
            //Array
            glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
        }

        err = glGetError();
        if (err != GL_NO_ERROR) {
            TraceLog(ErrorType::ERROR, "GL Error (after rendering): {}", err);
        }


    }

    void onShutdown() override {}
};

int main() {
    // Dexium context
    auto& ctx = EngineState::get();


    Dexium::Core::GLogger.outputs |= LogOutput::Stdout | LogOutput::PrettyPrint;
    VFS::overwriteExecPath(VFS::getExecutablePath().parent_path());

    ctx.attachWindow("Dexium Texture Test", 1080, 720);
    ctx.addLayer("game", std::make_unique<Game>());

    ctx.run();

    ctx.shutdown();
}