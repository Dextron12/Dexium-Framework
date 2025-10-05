

#ifndef APPLICATION_LAYER
#define APPLICATION_LAYER

#include <core/Mesh.hpp>
#include <core/Texture.hpp>
#include <core/Shader.hpp>


#include <core/Timer.hpp>
#include <core/Colour.hpp>
#include <core/config.hpp>
#include <core/helpers.h>

#include <core/window.hpp>
#include <core/AssetManager.hpp>
#include <core/Camera.hpp>
#include <core/Sprite.hpp>
#include <core/SpriteAnimations.hpp>
#include <core/Primitives.hpp>
#include <core/Material.hpp>

namespace Dexium {

    // This is a pretty dodgy class, only create this class as a std::shared_ptr<Layer> or it WILL throw an exception!
    class Layer {
    public:
        std::string ID;

        Layer(std::string LayerID);

        //User-hooks
        std::function<void()> onInit = nullptr; // default no-op
        std::function<void()> onUpdate = nullptr;
        std::function<void()> onRender = nullptr;


        void run(); // end-user SHOULD NEVER CALL THIS FUNCTION. Engine internally calls it

    private:
        bool init = false;
        bool paused = false;

    };


}
#endif // !- APPLICATION_LAYER