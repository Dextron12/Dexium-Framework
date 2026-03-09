//
// Created by Dextron12 on 5/3/26.
//

#ifndef DEXIUM_INPUT_HPP
#define DEXIUM_INPUT_HPP

#include <GLFW/glfw3.h>

#include <array>
namespace Dexium::Core {


    class Input {
    public:
        Input() = default;

        [[nodiscard]] bool isKeyDown(int key) const;
        [[nodiscard]] bool isKeyPressed(int key) const;
        [[nodiscard]] bool isKeyReleased(int key) const;

        // EngineState internally calls this. USer should never call it!
        void update();

        std::array<bool, GLFW_KEY_LAST + 1> m_currentKey{};
        std::array<bool, GLFW_KEY_LAST + 1> m_prevKey{};
    };


}


#endif //DEXIUM_INPUT_HPP