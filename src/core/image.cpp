#include "image.hpp"

Image::Image(int width, int height, ImageFormat imageFormat, std::vector<uint8_t> data) : m_width(width), m_height(height), m_imageFormat(imageFormat), m_data(data){}
std::vector<uint8_t> &Image::getData(){return m_data;}
int Image::getWidth(){return m_width;}
int Image::getHeight(){return m_height;}
ImageFormat Image::getFormat(){return m_imageFormat;}

// TODO : implement loading 8bit RGB/RGBA PNG only
Image Image::loadPNG(const string & path){
    LOGDEBUG("Loading image : {}", path);
    
}