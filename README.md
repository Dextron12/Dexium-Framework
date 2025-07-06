# Dexium 2D

[Dexium 2D](https://github.com/Dextron12/Dexium2D) is a lightweight 2D game framework built on **SDL2**. It aims to abstract away boilerplate while streamlining development for 2D games. While still in early development, Dexium 2D already includes a number of foundational features.

---

## ⚠️ Disclaimer

Dexium 2D is a personal project under active development. Features, structure, and APIs may change without notice as the framework evolves. Please expect occasional breaking changes until a stable release is tagged.

---

## 🎮 Window & Rendering System

- **Logical Resolution Scaling**: Defaults to 1080p logical resolution. The window can be resized while maintaining the logical size.
- **Double Buffer Rendering**:
  - Call `WindowContext::startFrame(int dt)` at the beginning of your game loop.
  - Call `WindowContext::endFrame()` to automatically swap buffers and apply logical scaling.
- **Event Management**
- **Timers**

---

## 📦 Asset Management

Dexium 2D includes a robust, extensible **Asset Manager** for handling game resources. It currently supports:

- ✅ **Textures**
- ✅ **Spritesheets** (with sub-texture rendering)
- ✅ **Animated Sprites** (built on top of the spritesheet system)

### 📌 Planned Support:
- 🎵 Music (streaming & background)
- 🔊 Multi-channel sound effects
- 📁 Packed assets via `.dexpak` files
- 🧩 Animated Modular Sprites (via **SpriteFoundry** only)
- 👤 Animated Player character (including state-driven animations)
- 🧱 Basic physics (AABB collisions and resolution)

---

### Lazy-loading Example:
```cpp
const auto& assets = AssetManager::get();

assets.init(WindowContext::renderer); // Renderer must be initialized before loading any asset.

assets.registerAsset<Texture>("pig", "Assets/Animals/Pig/Pig_01.png");
assets.registerAsset<Spritesheet>("Player", "Assets/Characters/Player_01.png");

const auto& pig = assets.use<Texture>("pig");          // Lazy-loaded on first use
const auto& player = assets.use<Spritesheet>("Player"); // Also lazy-loaded
```

---

### 🎞 Spritesheet & Sub-Texture Example:
```cpp
player->addSubTexture("idle", "Player", {0, 0, 32, 32});
player->render(WindowContext::renderer, "idle", {250, 300});
```

> If a registered asset isn't found, Dexium falls back to a checkered texture for error visibility.

---

### 🔗 Cross-asset dependency support:
```cpp
assets.registerAsset<Texture>("foodSheet", "Assets/Foodstuffs/food_noOutline.png");
assets.registerAsset<Spritesheet>("food");

// Will automatically load the `foodSheet` texture and link it to the `food` spritesheet.
const auto& food = assets.use<Spritesheet>("food");

food->addSubTexture("bread", "food", {64, 128, 32, 32});
food->render(WindowContext::renderer, "bread", {50, 50});
```

---

## 🧰 SpriteFoundry Tools

Dexium 2D is supported by an external toolset called [**SpriteFoundry**](https://github.com/Dextron12/SpriteFoundry). These tools include:

- 🧵 A GUI animation & rigging editor
- 🛠 A CLI asset packager
- 📦 Support for modular animated sprites and multi-texture setups
- 🧷 Anchor point definition and export

All assets can be exported into a custom `.dexpak` format — Dexium’s native binary asset format — which includes all textures, metadata, and animation data for a full game build.

> The goal of **SpriteFoundry** is to streamline asset registration and provide artist/developer-friendly tools for creating game-ready packs.

---

## 📁 Repository

📦 [Dexium 2D GitHub Repo](https://github.com/Dextron12/Dexium2D)

🔧 [SpriteFoundry Tools](https://github.com/Dextron12/SpriteFoundry)

---

If you’re looking to contribute, test, or fork Dexium 2D — feel free to open an issue or suggestion!