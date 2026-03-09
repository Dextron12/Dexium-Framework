//
// Created by Dextron12 on 5/3/26.
//

#include <core/Input.hpp>

#include <fmt/base.h>

namespace Dexium::Core {
    void Input::update() {
        m_prevKey = m_currentKey;
    }

    bool Input::isKeyDown(int key) const {
        return m_currentKey[key];
    }


    bool Input::isKeyPressed(int key) const {
        //fmt::print("Key Value: {}\n", m_currentKey[key]);
        return m_currentKey[key] && !m_prevKey[key];
    }

    bool Input::isKeyReleased(int key) const {
        return !m_currentKey[key] && m_prevKey[key];
    }
}
