#include "window.hpp"
#include "SpriteAnimations.hpp"
#include "worldBuilder.hpp"

#include <unordered_set>

#include "AssetManager.hpp"

/*
enum class TreeType {
    Stump,
    Oak,
    Spruce,
    Birch
};

struct PairHash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};


TreeType randomTree() {
    static const std::vector<TreeType> options = {
        TreeType::Stump,
        TreeType::Oak, TreeType::Oak, TreeType::Oak,
        TreeType::Spruce, TreeType::Spruce,
        TreeType::Birch
    };

    int index = rand() % options.size();
    return options[index];
}

Room town = generateRoom({ 80,45 }, [](Room& r) {
    // Pond settings
    int radius = 6;
    int centerX = r.roomSize.x - 12;
    int centerY = r.roomSize.y - 12;
    std::unordered_set<std::pair<int, int>, PairHash> pondCoords;

    auto isWaterAt = [&](int x, int y) -> bool {
        return pondCoords.count({ x, y }) > 0;
        };

    auto getBlobKey = [&](int x, int y) -> std::string {
        bool up = isWaterAt(x, y - 1);
        bool down = isWaterAt(x, y + 1);
        bool left = isWaterAt(x - 1, y);
        bool right = isWaterAt(x + 1, y);

        bool upLeft = isWaterAt(x - 1, y - 1);
        bool upRight = isWaterAt(x + 1, y - 1);
        bool downLeft = isWaterAt(x - 1, y + 1);
        bool downRight = isWaterAt(x + 1, y + 1);

        int count = up + down + left + right + upLeft + upRight + downLeft + downRight;

        // Mid-edge fix: prevent single-arm sticking out
        if ((up && down && left && right) || count >= 6) {
            return "centerCenter";
        }

        // Prioritize edges with no opposite neighbor
        if (!up && down && left && right) return "topCenter";
        if (up && !down && left && right) return "bottomCenter";
        if (up && down && !left && right) return "centerLeft";
        if (up && down && left && !right) return "centerRight";

        if (!up && down && !left && right) return "topLeft";
        if (!up && down && left && !right) return "topRight";
        if (up && !down && !left && right) return "bottomLeft";
        if (up && !down && left && !right) return "bottomRight";

        return "centerCenter"; // Default to solid center if unknown
        };



    // Pass 1: mark water tiles
    for (int y = centerY - radius; y <= centerY + radius; ++y) {
        for (int x = centerX - radius; x <= centerX + radius; ++x) {
            if (x < 0 || y < 0 || x >= r.roomSize.x || y >= r.roomSize.y) continue;

            int dx = x - centerX;
            int dy = y - centerY;

            if (dx * dx + dy * dy <= radius * radius) {
                pondCoords.emplace(x, y);
            }
        }
    }

    // Pass 2: draw pond
    for (const auto& [x, y] : pondCoords) {
        std::string animKey = getBlobKey(x, y);
        Tile t;
        t.tileID = "waterAnim";
        t.subTexture = animKey;
        t.pos = { x, y };
        r.tileMap[{x, y, Layer::Ground}] = t;
    }

    // ✅ Pass 3: fringe cleanup fix
    for (const auto& [x, y] : pondCoords) {
        int neighbors = 0;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                if (pondCoords.count({ x + dx, y + dy }) > 0) {
                    neighbors++;
                }
            }
        }

        if (neighbors < 3) {
            r.tileMap.erase({ x, y, Layer::Ground });
        }
    }

    // Pass 4: Fill grass + borders
    for (int y = 0; y < r.roomSize.y; y++) {
        for (int x = 0; x < r.roomSize.x; x++) {
            Tile tile;

            // Skip if a water tile is present
            auto groundKey = std::make_tuple(x, y, Layer::Ground);
            if (r.tileMap.count(groundKey) > 0) continue;

            tile.tileID = "grassAtlas";
            tile.subTexture = "grass_middle";
            tile.pos = { x, y };

            r.tileMap[groundKey] = tile;

            // Add tree decor to borders
            bool onVertBorder = x < 4 || x >= r.roomSize.x - 4;
            bool onHorBorder = y < 4 || y >= r.roomSize.y - 4;

            if (onVertBorder || onHorBorder) {
                TreeType choice = randomTree();

                if (choice != TreeType::Stump && y > 0) {
                    auto aboveKey = std::make_tuple(x, y - 1, Layer::Decor);
                    r.tileMap.erase(aboveKey);
                }

                Tile decor;
                decor.pos = { x, y };

                switch (choice) {
                case TreeType::Stump:
                    decor.tileID = "tree_oak";
                    decor.subTexture = "treeOak_stump";
                    break;
                case TreeType::Oak:
                    decor.tileID = "tree_oak";
                    decor.subTexture = "treeOak_tree";
                    break;
                case TreeType::Spruce:
                    decor.tileID = "tree_spruce";
                    decor.subTexture = "treeSpruce_tree";
                    break;
                case TreeType::Birch:
                    decor.tileID = "tree_birch";
                    decor.subTexture = "treeBirch_tree";
                    break;
                }

                if (onHorBorder && rand() % 100 < 30) continue; // 30% chance to skip horizontal
                r.tileMap[{x, y, Layer::Decor}] = decor;
            }
        }
    }
    });

*/



int main(int argc, char* argv[]) {
    VFS::init();

    WindowContext win("Hymn of the Blightest Stone", { 1280,720 }, SDL_WINDOW_RESIZABLE);
    auto& assets = AssetManager::get();
    assets.init(win.renderer);

    assets.registerAsset<Texture>("pig1", "Assets/Animals/Pig/Pig_01.png");
    assets.registerAsset<Spritesheet>("pig2", "Assets/Animals/Pig/Pig_02.png");

    //auto tex = assets.use<Texture>("pig1");
    auto tex = assets.use<Spritesheet>("pig2");
    tex->addSubTexture("idle", "pig2_spritesheet", { 0,0,32,32 });
    tex->addSubTexture("idle1", "pig2_spritesheet", { 32,0,32,32 });
    tex->addSubTexture("pig1_idle", "pig1", { 0,0,32,32 });

    Timer t(500);
    bool state = false;


    while (win.appState) {
        win.startFrame();

        SDL_SetRenderDrawColor(win.renderer, 40, 40, 40, 255);
        SDL_RenderClear(win.renderer);

        if (state) {
            tex->render(win.renderer, "pig1_idle", { 250,300 });
        }
        else {
            tex->render(win.renderer, "idle", { 250,300 });
        }

        if (t.isFinished()) {
            state = !state;
            t.reset();
        }



        win.endFrame();
    }
}

/*
int main(int argc, char* argv[]){
    VFS::init();
    

    WindowContext win("Hymn of the Blightest Stone", {1280, 720}, SDL_WINDOW_RESIZABLE);
    loadTextures(win.renderer); // *NOTE: Not the framework loadTexture()! *


    //SpriteAnimation player(win.renderer, "playerAnim", VFS::resolve("/Assets/Player/Player_New/Player_Anim/Player_Idle_Run_Death_Anim.png").string());
    //loadAnimationsFromJSON(VFS::resolve("/Assets/Animation scripts/Player/Player_idle.json").string(), player);
    //player.setAnimation("walk_forward");

    AnimatedPlayer player(win.renderer, "playerAnim", VFS::resolve("/Assets/Player/Player_New/Player_Anim/Player_Idle_Run_Death_Anim.png").string(), {(float)LOGICAL_WIDTH / 2, (float)LOGICAL_HEIGHT / 2});
    player.initAnimations(VFS::resolve("/Assets/Animation scripts/Player/Player_idle.json").string());
    Camera cam;

    auto& assets = AssetManager::get();
    assets.init(win.renderer);

    //auto t = assets.load<Texture>("pig", VFS::resolve("/Assetss/Animals/Pig/Pig_01.png").string());
    
    //assets.use<Texture>("pig");

    assets.registerAssetsFromManifest(VFS::resolve("Assets/manifest.json").string());

    if (assets.queryAsset<Texture>("pig")) {
        log(INFO, "The pig asset exists!");
    }

    SDL_Rect tPos;
    auto& texture = assets.use<Texture>("pig");
    tPos.w = texture->w; tPos.h = texture->h;
    tPos.x = 200; tPos.y = 300;



    while (win.appState){
        win.startFrame();

        SDL_SetRenderDrawColor(win.renderer, 40, 40, 40, 255);
        SDL_RenderClear(win.renderer);

        renderRoom(town);

        SDL_RenderCopy(win.renderer, texture->tex, nullptr, &tPos);

        //player.play({200,300});
        player.update(win.deltaTime);
        player.render(cam);

        //std::printf("CamX: %d, CamY: %d\n", cam.pos.x, cam.pos.y);

        if (TextureMngr::queryTexture("tree_oak")){
            auto spr = getSpritesheet(res, "tree_oak");
            spr->render("treeOak_tree", { 250,300 });
        }

        auto water = getSpriteAnim(resAnim, "waterAnim");
        water->setAnimation("topCenter");
        water->play({ 200,400 });

        auto tr = getSpritesheet(res, "tree_spruce");
        tr->render("treeSpruce_tree", { 200, 300 });

        auto br = getSpritesheet(res, "tree_birch");
        br->render("treeBirch_tree", { 300,300 });

        win.endFrame();
    }

    return 0;
}

*/