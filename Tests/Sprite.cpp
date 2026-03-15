
#include <Dexium.hpp>

#include "core/Input.hpp"
#include <core/Material.hpp>
#include <core/Texture.hpp>

#include <core/ResourcePool.hpp>

#include <glm/gtc/matrix_transform.hpp>

void printMat4x4(const glm::mat4& mat);

class Game : public Dexium::Core::GameLayer {
public:
    EngineState& ctx;
    Dexium::Core::WindowContext& window;
    Renderer renderer;

    Shader shader;
    Dexium::Core::Mesh mesh;
    Dexium::Core::Texture tex;
    Dexium::Core::Material mat;
    Dexium::Core::Transform transform;

    Dexium::Core::ResourceManager rm;
    Dexium::Core::ResourceHandle<Dexium::Core::Shader> resShader;
    Dexium::Core::ResourceHandle<Dexium::Core::Mesh> resMesh;
    Dexium::Core::ResourceHandle<Dexium::Core::Material> resMat;
    Dexium::Core::ResourceHandle<Dexium::Core::Texture> resTex;
    Dexium::Core::Transform resTrans;


    Game ()
        : ctx(EngineState::get())
    , window(ctx.getWindowContext())
    , renderer(&window) {}

    void onInit() override {
        shader = Shader("Shaders/basic.vert", "Shaders/basic.frag");
        shader.compile();
        mat.shader = &shader;

        resMat = rm.store(std::make_unique<Dexium::Core::Material>());
        rm.get(resMat)->shader = &shader;

        //auto meshHandle = rm.store<Dexium::Core::Mesh>(Dexium::Core::createMesh(Dexium::Core::MeshType::Mesh2D::Rectangle));
        resShader = rm.store<Dexium::Core::Shader>(std::make_unique<Shader>("", "", false));
        rm.get(resShader)->compile();

        resMesh = rm.store(Dexium::Core::createMesh(Dexium::Core::MeshType::Mesh2D::Rectangle));
        rm.get(resMesh)->buildMesh();

        //Set Transform
        transform.position = glm::vec3(250.f, 250.f, 0.f);
        transform.scale = glm::vec3(1.f);
        transform.rotation = glm::vec3(0.f, 0.f, 65.f);

        resTrans.position = glm::vec3(400.f, 200.f, 0.f);
        resTrans.scale = glm::vec3(1.f);
        resTrans.rotation = glm::vec3(0.f);

        //Set Texture filters
        tex.flags |= Dexium::Core::TexFlags::Nearest | Dexium::Core::TexFlags::Linear;
        tex.load("Cute_Fantasy/Buildings/Buildings/Houses/Limestone/House_2_Limestone_Base_Black.png");
        mat.setTexture("uTexture", &tex);

        resTex = rm.store(std::make_unique<Dexium::Core::Texture>());
        rm.get(resTex)->flags |= Dexium::Core::TexFlags::Nearest | Dexium::Core::TexFlags::Repeat;
        rm.get(resTex)->load("Cute_Fantasy/Outdoor decoration/Well.png");
        rm.get(resMat)->setTexture("uTexture", rm.get(resTex));

        //Scale model to texture size
        transform.scale = glm::vec3(tex.width, tex.height, 0.f);

        resTrans.scale = glm::vec3(rm.get(resTex)->width, rm.get(resTex)->height, 0.f) * 4.f;

        //Create Mesh
        mesh = Dexium::Core::Mesh();
        mesh.vertices = {
            0.f, 0.f, 0.f,      0.f, 1.f,
            1.f, 0.f, 0.f,      1.f, 1.f,
            1.f, 1.f, 0.f,      1.f, 0.f,
            0.f, 1.f, 0.f,      0.f, 0.f
        };
        mesh.vertexCount = 4;
        mesh.indices = {
            0, 1, 2,
            2, 3, 0
        };
        mesh.indexCount = 6;
        mesh.buildMesh();

        auto& vp = window.getRenderTarget().m_viewport;
        auto proj = glm::ortho(static_cast<float>(vp.x), static_cast<float>(vp.w), static_cast<float>(vp.h), static_cast<float>(vp.y), -1.f, 1.f);

        mat.setUniform("u_Model", transform.ModelMatrix());
        mat.setUniform("u_Projection", proj);

        rm.get(resMat)->setUniform("u_Model", resTrans.ModelMatrix());
        rm.get(resMat)->setUniform("u_Projection", proj);

        TraceLog(LogLevel::STATUS, "Begin output of Model Matrix (4x4)");
        printMat4x4(transform.ModelMatrix());
        TraceLog(LogLevel::STATUS, "End output of Model Matrix (4x4)");

        TraceLog(LogLevel::STATUS, "Begin output of Projection Matrix (4x4)");
        printMat4x4(proj);
        TraceLog(LogLevel::STATUS, "End output of Projection Matrix (4x4)");
    }

    void onUpdate() override {

        if (window.getInput().isKeyDown(GLFW_KEY_ESCAPE)) {
            ctx.shutdown();
        }
    }

    void onRender() override {
        renderer.setClearColor({51,51,51,255});

        renderer.submit({
        &window.getRenderTarget(),
        &mesh,
        &mat,
        &transform,
        Dexium::Core::RenderPass::Transparent});

        renderer.submit({
        &window.getRenderTarget(),
        rm.get(resMesh),
        rm.get(resMat),
        &resTrans,
        Dexium::Core::RenderPass::Transparent});

        renderer.flush();
    }

    void onShutdown() override {}
};


int main() {

    auto& ctx = EngineState::get();

    //Configure the engine logger system
    createLogger(Dexium::Core::LoggerOutput::Stderr, Dexium::Core::LoggerFormat::PrettyPrint);

    //Overwrites exec path to projectDir
    VFS::overwriteExecPath(VFS::getExecutablePath().parent_path());

    ctx.attachWindow("Dexium Sprite Test", 1080, 720);

    ctx.addLayer("game", std::make_unique<Game>());

    ctx.run();

    // No longer needed as of 0.20.x-a, engine internally calls this regardless of how it recieves the termiantion signal
    //ctx.shutdown();
}


void printMat4x4(const glm::mat4& mat) {
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            fmt::print(stderr, "{} ", mat[col][row]);
        }
        fmt::print(stderr, "\n");
    }
}