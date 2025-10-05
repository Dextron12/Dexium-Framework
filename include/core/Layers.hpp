

#ifndef APPLICATION_LAYER
#define APPLICATION_LAYER


#include <core/helpers.h>

#include <core/WindowContext.hpp>


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