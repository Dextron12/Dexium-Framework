
#include <Dexium.hpp>

#include "core/Texture.hpp"

#include <core/Material.hpp>

#include <core/Renderer.hpp>

#include <chrono>

#include "core/Transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

/*
std::vector<float> vertices = {
    // positions      // texCoords
    -0.5f, -0.5f, 0.f,  0.f, 0.f,
     0.5f, -0.5f, 0.f,  1.f, 0.f,
     0.5f,  0.5f, 0.f,  1.f, 1.f,
    -0.5f,  0.5f, 0.f,  0.f, 1.f
};

std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };
*/

class Game : public Dexium::Core::GameLayer {
    public:
    EngineState& ctx;

    Renderer renderer;

    Game()
        : ctx(EngineState::get())
        , renderer(&ctx.getWindowContext()) {}

    Shader shader;
    Dexium::Core::Mesh mesh;
    Dexium::Core::Texture tex;
    Dexium::Core::Material mat;
    Dexium::Core::Transform pos{
    glm::vec3(-80.f, 205.f, 0.f),
    glm::vec3(0,0,0),
    glm::vec3(4.f, 2.f, 1.f)};

    Dexium::RenderState::Viewport viewport;//(ctx.getWindowContext(), 0, 0, 1080, 720);
    Dexium::RenderState::RenderTarget* target;

    Dexium::Utils::MonoClock mc;

    void onInit() override {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        viewport = Dexium::RenderState::Viewport( 0, 0, ctx.getWindowContext().width, ctx.getWindowContext().height);

        //clearColour(Colour::fromBytes(59, 59, 59, 255));
        renderer.setClearColor({59, 59, 59, 255}, Dexium::Core::bufferTargets::Color);


        shader = Shader("Shaders/basic.vert", "Shaders/basic.frag");
        shader.compile();
        mat.shader = &shader;

        // Renderer is now respnosible for shader binds
        //shader.bind();

        const auto& winCtx = ctx.getWindowContext();
        auto projection = glm::mat4(1.0f);
        //projection = glm::ortho(-1,1,-1,1, -1,1);
        projection = glm::ortho(
            0.0f, 1080.f,           //left, right
            720.f, 0.f,          //bottom, top (flip Y)
            -1.0f, 1.0f           //near, far
            );
        //auto projection = glm::mat4(1.0);
        //projection = glm::ortho(0.0f, float(winCtx.width), float(winCtx.height), 0.0f, -1.0f, 1.0f);

        /*glm::mat4 ndcToPx(1.0f);
        float width = static_cast<float>(winCtx.width);
        float height = static_cast<float>(winCtx.height);
        ndcToPx[0][0] = width / 2.0f;       //Scale x
        ndcToPx[1][1] = -height / 2.0f;     //Scale y & flip
        ndcToPx[3][0] = width / 2.0f;       //Translate x
        ndcToPx[3][1] = height / 2.0f;      //Translate y
        ndcToPx[2][2] = 1.0f;               // Z unchanged
        ndcToPx[3][2] = 0.0f;               // Z translation
        */

        tex.flags |= Dexium::Core::TexFlags::Nearest | Dexium::Core::TexFlags::Repeat | Dexium::Core::TexFlags::Mipmaps;
        tex.load("Cute_Fantasy/Buildings/Buildings/Houses/Limestone/House_2_Limestone_Base_Black.png");


        mesh = Dexium::Core::Mesh();
        mesh.vertices = {
            // Pos                                          // UVs
            0.0f, 0.0f, 0.0f,                               0.f ,1.f,
            float(tex.width), 0.f, 0.0f,                    1.f, 1.f,
            float(tex.width), float(tex.height), 0.0f,      1.f, 0.f,
            0.f, float(tex.height), 0.f,                    0.f, 0.f
        };
        mesh.vertexCount = 4;
        mesh.indices = {
            0, 1, 2,
            2, 3, 0
        };
        mesh.indexCount = 6;
        mesh.buildMesh();
        /*mesh = *Dexium::Core::createMesh(Dexium::Core::MeshType::Mesh2D::Rectangle);
        mesh.vertices = vertices;
        mesh.indices = indices;
        */

        mat.setUniform("u_Model", pos.ModelMatrix());
        mat.setUniform("u_Projection", projection);

        fmt::print(stderr, "Projection Matrix: {}\n", glm::to_string(projection));
        fmt::print(stderr, "Model Matrix: {}\n", glm::to_string(pos.ModelMatrix()));

        fmt::print(stderr, "Pos: X: {}, Y: {}, Z: {}\n", pos.position.x, pos.position.y, pos.position.z);
        fmt::print(stderr, "Rot: X: {}, Y: {}, Z: {}\n", pos.rotation.x, pos.rotation.y, pos.rotation.z);
        fmt::print(stderr, "Sca: X: {}, Y: {}, Z: {}\n", pos.scale.x, pos.scale.y, pos.scale.z);

        const auto& verts = mesh.vertices;
        fmt::print(stderr, "Vert 0: X: {}, Y: {}, Z: {}, U: {}, V: {}\n", verts[0], verts[1], verts[2], verts[3], verts[4]);
        fmt::print(stderr, "Vert 1: X: {}, Y: {}, Z: {}, U: {}, V: {}\n", verts[5], verts[6], verts[7], verts[8], verts[9]);
        fmt::print(stderr, "Vert 2: X: {}, Y: {}, Z: {}, U: {}, V: {}\n", verts[10], verts[11], verts[12], verts[13], verts[14]);
        fmt::print(stderr, "Vert 3: X: {}, Y: {}, Z: {}, U: {}, V: {}\n", verts[15], verts[16], verts[17], verts[18], verts[19]);

        TraceLog(LogLevel::DEBUG, "Max supported texture slots: {}", renderer.pollHW_MaxTexSlots());
    }


    void onUpdate() override {

        if (glfwGetKey(ctx.getWindowContext().window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(ctx.getWindowContext().window, GLFW_TRUE);
        }

        mc.update();

        if (glfwGetKey(ctx.getWindowContext().window, GLFW_KEY_P) == GLFW_PRESS) {
            TraceLog(LogLevel::DEBUG, "Duration since last frame: {}", mc.delta());
        }
    }

    void onRender() override {

        Dexium::Core::RenderCommand comm(
            ctx.getWindowContext().getRenderTarget()
            , &mesh
            , &mat
            , &pos);
        renderer.submit(comm);

        //Stop Drawing(Deferred renderer, if manually drawing with GL, draw after this func call)
        renderer.flush();
    }

    void onShutdown() override {}
};

int main() {
    //Get global context of engine
    auto& ctx = EngineState::get();

    // Configure engine G-Logger
    //Dexium::Core::GLogger.outputs |= LogOutput::Stdout | LogOutput::PrettyPrint;
    createLogger(Dexium::Core::LoggerOutput::Stderr, Dexium::Core::LoggerFormat::PrettyPrint);

    if (Dexium::Core::LogService::use()) {
        TraceLog(LogLevel::STATUS, Dexium::Core::LoggerOutput::Stderr, "Logger is loaded");
    }

    // Overwrite execPath to working dir
    VFS::overwriteExecPath(VFS::getExecutablePath().parent_path());

    ctx.attachWindow("Dexium Command Textures", 1080, 720);

    ctx.addLayer("game", std::make_unique<Game>());

    ctx.run();

    ctx.shutdown();
}