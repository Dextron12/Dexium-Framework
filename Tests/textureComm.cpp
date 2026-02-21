
#include <Dexium.hpp>

#include "core/Texture.hpp"

#include <chrono>


std::vector<float> vertices = {
    // positions      // texCoords
    -0.5f, -0.5f, 0.f,  0.f, 0.f,
     0.5f, -0.5f, 0.f,  1.f, 0.f,
     0.5f,  0.5f, 0.f,  1.f, 1.f,
    -0.5f,  0.5f, 0.f,  0.f, 1.f
};

std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

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

    Dexium::Core::Viewport viewport;//(ctx.getWindowContext(), 0, 0, 1080, 720);

    std::chrono::steady_clock::time_point startTime;

    void onInit() override {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        viewport = Dexium::Core::Viewport(ctx.getWindowContext(), 0, 0, 1080, 720);

        clearColour(Colour::fromBytes(59, 59, 59, 255));


        shader = Shader("Shaders/basic.vert", "Shaders/basic.frag");
        shader.compile();
        mat.shader = &shader;

        // Renderer is now respnosible for shader binds
        //shader.bind();

        mesh = *Dexium::Core::createMesh(Dexium::Core::MeshType::Mesh2D::Rectangle);
        mesh.vertices = vertices;
        mesh.indices = indices;

        tex.flags |= Dexium::Core::TexFlags::Nearest | Dexium::Core::TexFlags::Repeat | Dexium::Core::TexFlags::Mipmaps;
        tex.load("Cute_Fantasy/Buildings/Buildings/Houses/Limestone/House_2_Limestone_Base_Black.png");

        startTime = std::chrono::steady_clock::now();
    }


    void onUpdate() override {

        if (glfwGetKey(ctx.getWindowContext().window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(ctx.getWindowContext().window, GLFW_TRUE);
        }

        if (glfwGetKey(ctx.getWindowContext().window, GLFW_KEY_P) == GLFW_PRESS) {
            std::chrono::duration<double> dur = std::chrono::steady_clock::now() - startTime;
            startTime = std::chrono::steady_clock::now();
            TraceLog(LogLevel::DEBUG, "Duration since last frame: {}", dur.count());
        }
    }

    void onRender() override {

        Dexium::Core::RenderCommand comm;
        comm.mesh = &mesh;
        comm.material = &mat;
        auto pos = Dexium::Core::Transform(glm::vec3(250,250,0));
        comm.transform = &pos;
        //comm.viewport = &viewport;
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
    createLogger(Dexium::Core::LoggerOutput::Stderr, Dexium::Core::LoggerFormat::PrettyPrint | Dexium::Core::LoggerFormat::PrefixLogLevels);

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