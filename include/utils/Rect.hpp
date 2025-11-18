//
// Created by Dextron12 on 18/11/2025.
//

#ifndef DEXIUM_RECT_HPP
#define DEXIUM_RECT_HPP


namespace Dexium::Utils {

    struct Rect {
    public:
        float x = 0.0f; float y = 0.0f; float z = 0.0f; float h = 0.0f;

        // 2D planar refs
        float& width() {return z;}
        float& height() {return h;}

        // 3D planar refs
        float& w() {return h;}

        // 2D:
        Rect(float x, float y) : x(x), y(y) {}
        Rect(float _x, float _y, float _width, float _height) : x(_x), y(_y) {z = _width; h = _height;}

        // 3D:
        Rect(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
        Rect(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), h(_w) {}

    };

}


#endif //DEXIUM_RECT_HPP