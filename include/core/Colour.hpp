//
// Created by Dextron12 on 14/09/2025.
//

#ifndef DEXIUM_COLOUR_HPP
#define DEXIUM_COLOUR_HPP
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Colour {
public:

    // RGBA
    Colour(const float& r, const float& g, const float& b, const float& a);
    Colour(const glm::vec4& colour);

    // RGB
    Colour(const float& r, const float& g, const float& b);
    Colour(const glm::vec3& colour);

    // Converts from (0,255) to OpenGL linear range [0,1]
    glm::vec4 normalized() const;

private:
    glm::vec4 colour;
};


#endif //DEXIUM_COLOUR_HPP