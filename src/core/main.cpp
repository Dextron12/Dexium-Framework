
#include "window.hpp"
#include "Timer.hpp"

#include <AssetManager.hpp>
#include <Mesh.hpp>

#include <Material.hpp>

#include <Sprite.hpp>
#include <SpriteAnimations.hpp>

# include "helpers.h"
#include "Primitives.hpp"

float vertices[] = {
	 0.5f,  0.5f, 0.0f,  // top right
	 0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f   // top left 
};


unsigned int indices[] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

#include "stb_image.h"

void dumpGlState(const char* tag) {
	GLint currentProg = 0; glGetIntegerv(GL_CURRENT_PROGRAM, &currentProg);
	GLint boundVAO = 0; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &boundVAO);
	GLint activeTexture = 0; glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
	GLint boundTex0 = 0; glActiveTexture(GL_TEXTURE0); glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTex0);
	GLint boundTex1 = 0; glActiveTexture(GL_TEXTURE1); glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTex1);
	GLint stencil = glIsEnabled(GL_STENCIL_TEST);
	GLint depth = glIsEnabled(GL_DEPTH_TEST);
	GLint blend = glIsEnabled(GL_BLEND);

	TraceLog(LOG_INFO, "[GL %s] prog=%d, VAO=%d, activeTex=%d, boundT0=%d, boundT1=%d, stencil=%d, depth=%d, blend=%d",
		tag, currentProg, boundVAO, activeTexture - GL_TEXTURE0, boundTex0, boundTex1, stencil, depth, blend);

	// Capture any extra errors
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) TraceLog(LOG_ERROR, "[GL %s] extra error: 0x%X", tag, err);
}

int main(int argc, char* argv[]) {
	WindowContext win("Dexium tests", 1280, 720);


	auto& assets = AssetManager::getInstance();
	assets.registerDefaultLoaders();
	assets.registerAsset("__2D-Shader", AssetType::Shader, { SHADER_2D_VERTEX, SHADER_2D_FRAGMENT }, false, false);

	Timer UIKey_Debounce(300);

	auto camera = std::make_shared<Camera>(win.width, win.height, ORTHOGRAPHIC);
	win.setWindowResizeCallback([&win, &camera] {
		//TraceLog(LOG_INFO, "[Window-Custom-Script] W: %i, H: %i", win.width, win.height);
		camera->setProjection(win.width, win.height, 0.0f, 1.0f);
	});

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto whiteTexture = []() -> Texture2D {
		Texture2D texture;
		glGenTextures(1, &texture.ID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture.ID);

		unsigned char whitePixel[4] = {255,255,255,255};
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

		return texture;
	};

	assets.registerAsset("__whiteTexture", whiteTexture());

	//Sprite boat("Textures/Boat.png", camera);
	//Sprite sheet("Textures/Player1.png", camera);
	//Sprite face("Textures/awesomeface.png", camera);
	Sprite boat("Textures/boat.png");
	boat.camera = camera;
	boat.setShader("__2D-Shader");

	Spritesheet tree("Textures/Big_Birch_Tree.png");
	tree.camera = camera;
	tree.setShader("__2D-Shader");
	tree.setSprite("stump", {0, 0, 32, 64});
	tree.setSprite("grown", {32, 0, 32, 80});

	AnimatedSprite player("Textures/Player1.png", camera);
	player.setShader("__2D-Shader");
	player.setSequence("front", // Sequence name
	{
		newSeqFrame("frame1", {16, 16, 32, 32}, 150),
		newSeqFrame("frame2", {80, 16, 32, 32}, 200),
		newSeqFrame("frame3", {144, 16, 32, 32}, 250),
		newSeqFrame("frame4", {208, 16, 32, 32}, 200),
		newSeqFrame("frame5", {272, 16, 32, 32}, 150),
		newSeqFrame("frame6", {336, 16, 32, 32}, 150),
		},
		320.0f, // default duration (used if duration == 0)
		true
		);


	Spritesheet t("Textures/Player1.png");
	t.camera = camera;
	t.setShader("__2D-Shader");
	//t.load(); // <-- Deprecated function!!
	t.setSprite("front0", {16,16,32,32});
	t.setSprite("front1", {80,16,32,32});
	t.setSprite("front2", {144, 16, 32, 32});
	t.setSprite("front3", {208, 16, 32, 32});
	t.setSprite("front4", {272, 16, 32, 32});
	t.setSprite("front5", {336, 16, 32, 32});

	auto mesh = MeshFactory::QuadWithUV();
	Transform transform({230,230,0});
	transform.position = {240,240,0};
	transform.scale = {32, 32, 1};

	auto shader = assets.use<Shader>("__2D-Shader");
	//shader->use();
	//shader->setMat4("projection", camera->getProjection());
	//shader->setMat4("model", transform.toModelMatrix());


	Timer anim(380);
	int animCount = 0;


	while (win.appState) {
		win.startFrame();

		if (glfwGetKey(win.window, GLFW_KEY_F1) == GLFW_PRESS && UIKey_Debounce.isFinished()) {
			win.toggleWireFrame();
			UIKey_Debounce.reset();
		}

		clearScreen(51, 76, 76, 255);
		//shader->setMat4("projection", camera->getProjection());

		boat.drawSprite({300.0f, 100.0f, 144.0f, 144.0f});


		//Primitives::FilledRect({250,250,350,20}, Colour(140,140,72), *shader.get());
		Primitives::Rect({15, 150, 600, 300}, Colour(255,0,0,255), *shader.get());


		if (anim.isFinished()) {
			if (animCount >= 5) { animCount = 0;}
			else {animCount++;}
			anim.reset();
		}

		tree.drawSprite("grown", {350, 150, 0});

		std::string ID = "front" + std::to_string(animCount);
		//std::string ID = "front3";
		t.drawSprite(ID, {250,250,0});

		//player.update("front", win.deltaTime);
		//player.render("front", {276,250,0,0});
		player.updateAndRender("front", win.deltaTime, {276, 250, 0, 0});



		//face.render({ 850, 250, 32, 32 });


		//boat.render(Transform({250, 250, 0}, {64, 32, 1}));

		//sheet.render({ 600,2000,576,3584 });

		win.endFrame();
	}
}



/*
int main(int argc, char* argv[]) {
	WindowContext win("Dexium Tests", 1280, 720);

	auto& assets = AssetManager::getInstance();
	assets.registerDefaultLoaders(); // Should eventually be moved to an engine init function
	
	// Register default assets
	assets.registerAsset("__2D-Shader", AssetType::Shader, { SHADER_2D_VERTEX, SHADER_2D_FRAGMENT }, false, false);

	Timer UIKey_Debounce(300);

	//glm::mat4 projection = glm::ortho(0.0f, (float)win.width, (float)win.height, 0.0f, -1.0f, 1.0f);
	Camera camera(win.width, win.height, ORTHOGRAPHIC);

	assets.registerAsset("2DShader", AssetType::Shader,
		{ VFS::resolve("Shaders/2D_vertex.glsl")->c_str(), VFS::resolve("Shaders/2D_fragment.glsl")->c_str()});

	assets.registerAsset("basicShader", AssetType::Shader,
		{ VFS::resolve("Shaders/basicVert.glsl")->c_str(), VFS::resolve("Shaders/basicFrag.glsl")->c_str() });

	assets.registerAsset("face", AssetType::Texture, { VFS::resolve("Textures/awesomeface.png")->c_str() });
	auto tex = assets.use<Texture2D>("face");

	assets.registerAsset("boat", AssetType::Texture, { VFS::resolve("Textures/Boat.png")->c_str() });
	auto tex1 = assets.use<Texture2D>("boat");

	//shader->setMat4("projection", projection);


	//mat.setTexture(Material::TextureType::Albedo, "boat");

	auto shader = assets.use<Shader>("2DShader");
	//shader->use();


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Renderable crate;
	crate.mesh = std::make_shared<Mesh>();
	crate.material = std::make_shared<Material>("2DShader");
	crate.transform = std::make_unique<Transform>(
		glm::vec3(400.0f, 200.0f, 0.0f),
		glm::vec3(32.0f, 32.0f, 1.0f)
	);

	crate.mesh->vertexCount = 4;
	crate.mesh->indexCount = 6;

	crate.material->setTexture(Material::TextureType::Albedo, "albedo", "face");
	crate.mesh->upload(vertices, sizeof(vertices), []() {
		// Pos
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// UVs
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(1);
		},
		indices, sizeof(indices));


	Renderable boat;
	boat.mesh = std::make_shared<Mesh>();
	boat.transform = std::make_unique<Transform>(
		glm::vec3(200.0f, 150.0f, 0.0f),
		glm::vec3(240.0f, 240.0f, 1.0f)
		);
	boat.material = std::make_shared<Material>("2DShader");

	const auto& m = boat.mesh;
	const auto& mat = boat.material;
	m->indexCount = 6;
	m->vertexCount = 4;

	mat->setTexture(Material::TextureType::Albedo, "albedo", "boat");

	m->upload(vertices, sizeof(vertices), []() {
		// Pos
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		/*
		// Colour
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		////

		// UVs
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(1);

		}, indices, sizeof(indices));

	mat->bind();

	Sprite box("Textures/Boat.png", std::make_shared<Camera>(camera));
	

	while (win.appState) {
		win.startFrame();

		if (glfwGetKey(win.window, GLFW_KEY_F1) == GLFW_PRESS && UIKey_Debounce.isFinished()) {
			win.toggleWireFrame();
			UIKey_Debounce.reset();
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		boat.render(camera.getProjection());

		crate.render(camera.getProjection());

		box.render({ 250.0f, 400.0f, 0.0f });

		//tex->render({450, 250, 32, 32});

		//tex1->render({ 200,400 });

		win.endFrame();
	}
}

*/

