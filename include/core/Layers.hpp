

#ifndef APPLICATION_LAYER
#define APPLICATION_LAYER

namespace Dexium {

    class AppState {
    public:

        // Make it non-copyable
        AppState(const AppState&) = delete;
        AppState& operator=(const AppState&) = delete;
        AppState(AppState&&) = default;
        AppState& operator=(AppState&&) = default;

        virtual void onInit() = 0;
        virtual void onUpdate() = 0;
        virtual void onRender() = 0;
        virtual void onShutdown() = 0; // Engine calls this function when layer is being removed

        virtual bool isOverlay() { return false; }

        void run(); // WARNING: end-user is not to call this function! Engine internally calls it!

        bool isActive() { return isRunning; }
        bool isInitialized() { return isInited; }
        bool isShuttingDown() { return isRunning && _isShutdown; }
        bool isShutdown() {return _isShutdown;}

        void RequestPause();
        void RequestShutdown();

    protected:
        bool isInited = false; // Becomes true after onInit has been executed
        bool isRunning = false; // If the layer is active
        bool _isShutdown = false;

        // Force heap-alloc (shared_ptr)
        AppState() = default;
        virtual ~AppState() = default;

    };


    class Overlay : public AppState {
    public:
        bool isOverlay() override { return true; };
    };

}

/*
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


*/
#endif // !- APPLICATION_LAYER