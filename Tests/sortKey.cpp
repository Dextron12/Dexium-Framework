#include <iostream>
#include <cstdint>

/*
enum class DepthOrder : uint8_t {
    FRONT_TO_BACK,      // Render near things first -> minimize overdaw
    BACK_TO_FRONT       // Render far things first -> correct blending
};
*/

enum class SortOrder : uint32_t {
    Shader = (unsigned int)1 << 31,
    Material = (unsigned int)1 << 23,
    Mesh = (unsigned int)1 << 15 // Really not needed, but used to differentiate keys

};

