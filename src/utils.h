#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <glm/glm.hpp>

static unsigned char floatToUnsignedChar(const float f) {
    return static_cast<unsigned char>(round(std::max(std::min(1.0f, f), 0.0f) * 255));
}

void write_matrix_to_png(const std::vector<glm::vec4> &pixels, int numRows, int numCols, const std::string& filename)
{
    const int w = numCols;
    const int h = numRows;

    const int comp = 4; 
    const int stride_in_bytes = w*comp;

    std::vector<unsigned char> data(w*h*comp,0);

    for (int wi = 0; wi < w; ++wi) {
        for (int hi = 0; hi < h; ++hi) {
            const glm::vec4 &p = pixels[hi*w+wi];
            data[(hi * w * 4) + (wi * 4) + 0] = floatToUnsignedChar(p.r);
            data[(hi * w * 4) + (wi * 4) + 1] = floatToUnsignedChar(p.g);
            data[(hi * w * 4) + (wi * 4) + 2] = floatToUnsignedChar(p.b);
            data[(hi * w * 4) + (wi * 4) + 3] = floatToUnsignedChar(p.a);
        }
    }

    stbi_write_png(filename.c_str(), w, h, comp, data.data(), stride_in_bytes);
}