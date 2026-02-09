//
// Created by Dextron12 on 3/12/25.
//

#ifndef DEXIUM_COLOUR_H
#define DEXIUM_COLOUR_H

#include <fmt/color.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Colour {
public:
    // Constructors --->

    // Float-space (0-1)
    Colour(float r, float g, float b, float a = 1.0f)
        : r_(r), g_(g), b_(b), a_(a) {}

    // Byte-space (0-255)
    static Colour fromBytes(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        return Colour(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }

    // allow fmt::rgb (always opaque)
    Colour (fmt::rgb c)
        : r_(c.r / 255.f), g_(c.g / 255.0f), b_(c.b / 255.0f), a_(1.0f) {}

    // Allow fmt::color (converted internally to rgb)
    Colour(fmt::color c)
        : Colour(static_cast<fmt::rgb>(c)) {}

    // Accessors

    float r() const { return r_; }
    float g() const { return g_; }
    float b() const { return b_; }
    float a() const { return a_; }
    glm::vec3 rgb() const { return glm::vec3(r_, g_, b_); }
    glm::vec4 rgba() const { return glm::vec4(r_, g_, b_, a_); }

    // Byte-space output (0-255 range)
    uint8_t rByte() const {return static_cast<uint8_t>(std::round(r_ * 255.f)); }
    uint8_t gByte() const {return static_cast<uint8_t>(std::round(g_ * 255.f)); }
    uint8_t bByte() const {return static_cast<uint8_t>(std::round(b_ * 255.f)); }
    uint8_t aByte() const {return static_cast<uint8_t>(std::round(a_ * 255.f)); }
    glm::vec3 rgbByte() const { return glm::vec3(rByte(), gByte(), bByte()); }
    glm::vec4 rgbaByte() const { return glm::vec4(rByte(), gByte(), bByte(), aByte()); }


private:
    float r_, g_, b_, a_;
};

// Im Not a US programmer, so I choose to spell Colour correctly
using Color = Colour;

#endif //DEXIUM_COLOUR_H