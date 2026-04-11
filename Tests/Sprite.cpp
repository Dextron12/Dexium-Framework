
#include <Dexium.hpp>

#include "core/Input.hpp"
#include <core/Material.hpp>
#include <core/Texture.hpp>
#include <core/Camera.hpp>

#include <core/ResourcePool.hpp>

#include <renderer/RenderPass.hpp>
#include <renderer/Presets.hpp>

#include <core/Model.hpp>

void printMat4x4(const std::string& matName, const glm::mat4& mat);

class Game : public Dexium::Core::GameLayer {
public:
    EngineState& ctx;
    Dexium::Core::WindowContext& window;
    Dexium::Renderer::Renderer renderer;

    Dexium::Renderer::RenderPass pass;

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

    Dexium::Core::Camera2D camera;

    Dexium::Core::Model house;




    Game ()
        : ctx(EngineState::get())
    , window(ctx.getWindowContext()) {}
    //, renderer(&window){}

    void onInit() override {
        shader = Shader("Shaders/basicVert.glsl", "Shaders/basicFrag.glsl");
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
        tex.flags |= Dexium::Utils::TexFlags::Nearest | Dexium::Utils::TexFlags::ClampEdge;
        tex.load("Cute_Fantasy/Buildings/Buildings/Houses/Limestone/House_2_Limestone_Base_Black.png");
        mat.setTexture("uTexture", &tex);

        resTex = rm.store(std::make_unique<Dexium::Core::Texture>());
        rm.get(resTex)->flags |= Dexium::Utils::TexFlags::Linear | Dexium::Utils::TexFlags::Linear;
        rm.get(resTex)->load("Cute_Fantasy/Outdoor decoration/Well.png");
        rm.get(resMat)->setTexture("uTexture", rm.get(resTex));

        auto handle = Dexium::Core::ResourceHandle<Dexium::Core::Texture>();
        auto test = rm.get(handle);
        if (test == nullptr) {
            TraceLog(LogLevel::ERROR, "Test handle is invalid, Fetched no content");
        }

        //Scale model to texture size
        transform.scale = glm::vec3(tex.width, tex.height, 0.f);

        resTrans.scale = glm::vec3(rm.get(resTex)->width, rm.get(resTex)->height, 0.f) * 4.f;

        camera = Dexium::Core::Camera2D();

        house = Dexium::Core::Model(
            *Dexium::Core::createMesh(Dexium::Core::MeshType::Mesh2D::Rectangle),
            Dexium::Core::Material(),
            Dexium::Core::Transform({450.f, 350.f, 0.f}, {0.f, 0.f, -45.f}, {tex.width, tex.height, 0.f})
            );
        house.m_Material->shader = &shader;
        house.m_Material->setTexture("uTexture", &tex);

        /*
        pass = Dexium::Renderer::RenderPass(&window.getRenderTarget(), &camera);
        pass.setClearColor(Color::fromBytes(47, 47, 47, 255));
        pass.plpState.Projection_uName = "u_Projection";
        pass.plpState.View_uName = "u_View";
        pass.plpState.Model_uName = "u_Model";

        pass.plpState.blending = true;
        */
        pass = Dexium::Renderer::Presets::TransparentPass(&window.getRenderTarget(), &camera);

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

        pass.storeCommand(&mesh, &mat, &transform);

        pass.storeCommand(rm.get(resMesh), rm.get(resMat), &resTrans);

        /*
        auto& vp = window.getRenderTarget().m_viewport;
        auto proj = glm::ortho(static_cast<float>(vp.x), static_cast<float>(vp.w), static_cast<float>(vp.h), static_cast<float>(vp.y), -1.f, 1.f);

        mat.setUniform("u_Model", transform.ModelMatrix());
        mat.setUniform("u_Projection", proj);
        mat.setUniform("u_View", camera.getViewMatrix());

        rm.get(resMat)->setUniform("u_Model", resTrans.ModelMatrix());
        //rm.get(resMat)->setUniform("u_Projection", proj);
        */

        printMat4x4("Projection", camera.getProjectionMatrix(window.getRenderTarget().m_viewport));
        printMat4x4("View", camera.getViewMatrix());
        printMat4x4("Model", camera.transform.ModelMatrix());
    }

    void onUpdate() override {

        if (window.getInput().isKeyDown(GLFW_KEY_ESCAPE)) {
            ctx.shutdown();
        }
    }

    void onRender() override {
        //renderer.setClearColor({51,51,51,255});

        pass.storeCommand(&mesh, &mat, &transform); // House
        pass.storeCommand(rm.get(resMesh), rm.get(resMat), &resTrans); // Well

        // Model driven:
        pass.storeCommand(house.m_Mesh.get(), house.m_Material.get(), house.m_Transform.get());

        renderer.submit(&pass);

        /*renderer.submit({
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
        Dexium::Core::RenderPass::Transparent});*/

        renderer.flush();
    }

    void onShutdown() override {}
};


int main() {

    auto& ctx = EngineState::get();

    //Configure the engine logger system
    createLogger(Dexium::Utils::LoggerOutput::Stderr, Dexium::Utils::LoggerFormat::PrettyPrint);

    //Overwrites exec path to projectDir
    VFS::overwriteExecPath(VFS::getExecutablePath().parent_path());

    ctx.attachWindow("Dexium Sprite Test", 1080, 720);

    ctx.addLayer("game", std::make_unique<Game>());

    ctx.run();

    // No longer needed as of 0.20.x-a, engine internally calls this regardless of how it recieves the termiantion signal
    //ctx.shutdown();
}


void printMat4x4(const std::string& matName, const glm::mat4& mat) {
    fmt::print(stderr, fg(fmt::color::lime_green), fmt::format(" --- Begin Output of {} (4x4 Mat) ---\n", matName));
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            fmt::print(stderr, fg(fmt::color::lime_green), "{} ", mat[col][row]);
        }
        fmt::print(stderr, "\n");
    }
    fmt::print(stderr, fg(fmt::color::lime_green), fmt::format(" --- End Output of {} (4x4 Mat) ---\n\n", matName));

}