#pragma once
#include "defines.hpp"
#include <cstdint>
#include <vector>

enum class ImageFormat {
    RGB,
    RGBA
};

class Image {
    private:
        int m_width;
        int m_height;
        ImageFormat m_imageFormat;
        std::vector<uint8_t> m_data;
    public:
        Image(int width, int height, ImageFormat imageFormat, std::vector<uint8_t> data);
        int getWidth();
        int getHeight();
        ImageFormat getFormat();
        std::vector<uint8_t> &getData();
        static Image loadPNG(const string &path);
};

