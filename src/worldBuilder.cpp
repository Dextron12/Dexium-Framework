#include "worldBuilder.hpp"

/*

std::unordered_map<std::string, Spritesheet> res;
std::unordered_map<std::string, SpriteAnimation> resAnim;


void loadTextures(SDL_Renderer* renderer) {

    //Create the grass spritesheet:
    res.insert_or_assign("grassAtlas", Spritesheet(renderer, "grassAtlas", VFS::resolve("/Assets/Tiles/Grass/Grass_Tiles_3.png").string()));
    //auto& spr = res["grassAtlas"];
    auto& spr = *getSpritesheet(res, "grassAtlas");

    // Grass Blob
    spr.pushSubTexture("grass_topLeft", { 0,0,16,16 });
    spr.pushSubTexture("grass_topCenter", { 16,0,16,16 });
    spr.pushSubTexture("grass_topRight", { 32,0,16,16 });

    spr.pushSubTexture("grass_centerLeft", { 0,16,16,16 });
    // No need for the centerTile its blank.
    spr.pushSubTexture("grass_centerRight", { 32,16,16,16 });

    spr.pushSubTexture("grass_bottomLeft", { 0,32,16,16 });
    spr.pushSubTexture("grass_bottomCenter", { 16,32,16,16 });
    spr.pushSubTexture("grass_bottomRight", { 32,32,16,16 });
    // End Grass Blob

    // Stone Blob
    spr.pushSubTexture("stone_topLeft", { 48,80,16,16 });
    spr.pushSubTexture("stone_topCenter", { 64,80,16,16 });
    spr.pushSubTexture("stone_topRight", { 80,80,16,16 });

    spr.pushSubTexture("stone_centerLeft", { 48,96,16,16 });
    // No need for the centerTile its blank.
    spr.pushSubTexture("stone_centerRight", { 80,96,16,16 });

    spr.pushSubTexture("stone_bottomLeft", { 48,112,16,16 });
    spr.pushSubTexture("stone_bottomCenter", { 64,112,16,16 });
    spr.pushSubTexture("stone_bottomRight", { 80,112,16,16 });
    // End Stone Blob

    // Dirt Blob
    spr.pushSubTexture("dirt_topLeft", { 48,0,16,16 });
    spr.pushSubTexture("dirt_topCenter", { 64,0,16,16 });
    spr.pushSubTexture("dirt_topRight", { 80,0,16,16 });

    spr.pushSubTexture("dirt_centerLeft", { 48,16,16,16 });
    // No need for the centerTile its blank.
    spr.pushSubTexture("dirt_centerRight", { 80,16,16,16 });

    spr.pushSubTexture("dirt_bottomLeft", { 48,32,16,16 });
    spr.pushSubTexture("dirt_bottomCenter", { 64,32,16,16 });
    spr.pushSubTexture("dirt_bottomRight", { 80,32,16,16 });
    // End dirt blob

    spr.addSpritesheet(renderer, "grass_middle", VFS::resolve("/Assets/Tiles/Grass/Grass_3_Middle.png").string());

    // Medium oak tree
    res.insert_or_assign("tree_oak", Spritesheet(renderer, "tree_oak", VFS::resolve("/Assets/Trees/Medium_Oak_Tree.png").string()));
    auto okTree = getSpritesheet(res, "tree_oak");
    okTree->pushSubTexture("treeOak_stump", { 0,16,32,32 });
    okTree->pushSubTexture("treeOak_tree", { 32,0,32,38 });

    // Medium Spruce Tree
    res.insert_or_assign("tree_spruce", Spritesheet(renderer, "tree_spruce", VFS::resolve("/Assets/Trees/Medium_Spruce_Tree.png").string()));
    auto sprTree = getSpritesheet(res, "tree_spruce");
    sprTree->pushSubTexture("treeSpruce_stump", { 0,16,32,32 });
    sprTree->pushSubTexture("treeSpruce_tree", { 32,0,32,38 });

    // Medium Birch Tree
    res.insert_or_assign("tree_birch", Spritesheet(renderer, "tree_birch", VFS::resolve("/Assets/Trees/Medium_Birch_Tree.png").string()));
    auto brTree = getSpritesheet(res, "tree_birch");
    brTree->pushSubTexture("treeBirch_stump", { 0,16,32,32 });
    brTree->pushSubTexture("treeBirch_tree", { 32,0,32,38 });

    // Water Anim
    resAnim.insert_or_assign("waterAnim", SpriteAnimation(renderer, "waterAnim", VFS::resolve("/Assets/Tiles/Water/Water_Tile_3_Anim.png").string()));
    auto water = getSpriteAnim(resAnim, "waterAnim");
    loadAnimationsFromJSON(VFS::resolve("Assets/Animation scripts/Tiles/stagWater.json").string(), *water);

    resAnim.insert_or_assign("waterAnim_middle", SpriteAnimation(renderer, "waterAnim_middle", VFS::resolve("/Assets/Tiles/Water/Water_Middle_Anim_2.png").string()));
    auto waterMiddle = getSpriteAnim(resAnim, "waterAnim_middle");
    //waterMiddle->addAnimation("middle", std::vector<AnimationFrame())

}

Spritesheet* getSpritesheet(std::unordered_map<std::string, Spritesheet>& res, const std::string& key) {
    auto it = res.find(key);
    return (it != res.end()) ? &it->second : nullptr;
}

SpriteAnimation* getSpriteAnim(std::unordered_map<std::string, SpriteAnimation>& res, const std::string& key) {
    auto it = res.find(key);
    return (it != res.end()) ? &it->second : nullptr;
}

Room generateRoom(SDL_Point roomSize, RoomGenFn genFn) {
    Room r;
    r.roomSize = roomSize;
    genFn(r);
    return r;
}

void renderRoom(const Room& room) {
    //Explicit renderOrder:
    std::vector<Layer> drawOrder = {
        Layer::Ground,
        Layer::Decor,
        Layer::Structure,
        Layer::Enemy
    };
    for (Layer currentLayer : drawOrder) {
        for (const auto& [key, tile] : room.tileMap) {
            int x = std::get<0>(key);
            int y = std::get<1>(key);
            Layer tileLayer = std::get<2>(key);

            if (tileLayer != currentLayer) {
                continue; // Move to the next tile.
            }

            // Retreive texture
            auto tex = getSpritesheet(res, tile.tileID);
            if (tex == nullptr) {
                tex = getSpriteAnim(resAnim, tile.tileID);
                if (tex == nullptr) {
                    std::printf("[Fatal Error]: Failed to fetch pre-loaded texture: '%s'\n", tile.tileID.c_str());
                    abort();
                }
            }

            if (tile.subTexture.empty()) {
                tex->render(tile.tileID, { x * TILE_SIZE, y * TILE_SIZE }, tile.tileID); // In this case, ussually rendering a subTexture entirely
            }
            else {
                if (tile.subTexture == "treeOak_stump") {
                    tex->render(tile.subTexture, { x * TILE_SIZE, y * TILE_SIZE });
                    continue;
                    /* Due to Spritesheet::render() limitations, specifically the subTexture unordered_map not holding the default texture of the Spritesheet obj
                    by trying to call the textureID in the optional 3rd param, results in the funciton dumping an error instead of resolving the inital texture
                }

                if (tile.subTexture == "treeOak_tree") {
                    tex->render(tile.subTexture, { x * TILE_SIZE, y * TILE_SIZE });
                    continue;
                }
                if (tile.subTexture == "treeSpruce_tree") {
                    tex->render(tile.subTexture, { x * TILE_SIZE, y * TILE_SIZE });
                    continue;
                }
                if (tile.subTexture == "treeBirch_tree") {
                    tex->render(tile.subTexture, { x * TILE_SIZE, y * TILE_SIZE });
                    continue;
                }
                if (tile.tileID == "waterAnim") {
                    auto anim = getSpriteAnim(resAnim, tile.tileID);
                    anim->setAnimation(tile.subTexture, true, false);
                    anim->play({ x * TILE_SIZE, y * TILE_SIZE });
                    continue;
                }

                //In all other cases:
                tex->render(tile.subTexture, { x * TILE_SIZE, y * TILE_SIZE }, tile.tileID);
            }
        }
    }
}

*/
