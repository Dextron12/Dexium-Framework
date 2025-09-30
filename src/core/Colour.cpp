//
// Created by Dextron12 on 14/09/2025.
//

#include "Colour.hpp"

Colour::Colour(const float& r, const float& g, const float& b, const float& a) {
    colour = glm::vec4(r, g, b, a);
}

Colour::Colour(const glm::vec4& colour) {
    this->colour = colour;
}

Colour::Colour(const float &r, const float &g, const float &b) {
    this->colour = glm::vec4(r, g, b, 255.0f);
}

Colour::Colour(const glm::vec3 &colour) {
    this->colour = glm::vec4(colour, 255.0f);
}




glm::vec4 Colour::normalized() const {
    glm::vec4 col;
    col.r = (float)colour.r / 255.0f;
    col.g = (float)colour.g / 255.0f;
    col.b = (float)colour.b / 255.0f;
    col.a = (float)colour.a / 255.0f;

    return col;
}


