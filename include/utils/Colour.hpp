//
// Created by Dextron12 on 18/11/2025.
//

#ifndef DEXIUM_COLOUR_HPP
#define DEXIUM_COLOUR_HPP

#include <variant>

#include "fmt/color.h"

struct Col {
    float R, G, B, A;

    Col(float R, float G, float B, float A) : R(R), G(G), B(B), A(A) {checkRange(R,G,B,A);}
    Col(float R, float G, float B) : R(R), G(G), B(B) {checkRange(R,G,B, 0);}

    // converts the colour range to [0-1] from [0-255]
    Col getNormalised() const {
        return Col{R / 255.f, G / 255.f, B / 255.f, A / 255.f};
    }

private:
    void checkRange(float r, float g, float b, float a) {
        if (r >= 0.f && r <= 1.f) {
            R = r * 255.f;
        }

        if (g >= 0 && g <= 1.f) {
            G = g * 255.0f;
        }

        if (b >= 0 && b <= 1.f) {
            B = b * 255.0f;
        }

        if (a >= 0 && a <= 1.f) {
            A = a * 255.0f;
        }
    }
};

class Colour {
    public:
        //std::variant<Col, fmt::color> ColourType; // Intake / TypeSpecifier

        //Always stores as Col internally
        Col value{0,0,0,0};

        Colour(std::variant<Col, fmt::color> input) {
            if (std::holds_alternative<Col>(input)) {
                value = std::get<Col>(input);
            } else if (std::holds_alternative<fmt::color>(input)) {
                const auto& rgb = fmt::rgb(std::get<fmt::color>(input));
                value = Col{static_cast<float>(rgb.r), static_cast<float>(rgb.g), static_cast<float>(rgb.b)};
            }
        }

};

#endif //DEXIUM_COLOUR_HPP