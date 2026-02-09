//
// Created by Dextron12 on 1/12/25.
//

#ifndef DEXIUM_LAYERS_HPP
#define DEXIUM_LAYERS_HPP

namespace Dexium::Core {

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

        AppState() = default;
        virtual ~AppState() = default;

    protected:
        bool isInited = false; // Becomes true after onInit has been executed
        bool isRunning = false; // If the layer is active
        bool _isShutdown = false;

    };

    using AppLayer = AppState;
    using GameLayer = AppState;
}

#endif //DEXIUM_LAYERS_HPP