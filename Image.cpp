#include <iostream>
#include "FreeImage.h"
#include "Image.h"

Image::Image() {
}

Image::Image(int _width, int _height) {
	width = _width;
	height = _height;
	FreeImage_Allocate(width, height, 24);
}

Image::~Image() {
}

void Image::load(const char *filePath) {
	m_fiImage = FreeImage_Load(FIF_PNG, filePath, PNG_DEFAULT);
	if (!m_fiImage) {
		std::cerr << "unsuccessful file load" << std::endl;
		exit(1);
	}

}

void Image::save(const char *filePath) {
	if (!FreeImage_Save(FIF_PNG, m_fiImage, filePath, 0)) {
		std::cerr << "Saving file " << filePath << " unsuccessful" << std::endl;
		exit(1);
	}
}

void Image::getPixel(RGBQUAD &rgb, int x, int y) const {
	FreeImage_GetPixelColor(m_fiImage, x, y, &rgb);
}

void Image::setPixel(RGBQUAD &rgb, int x, int y) const {
	FreeImage_SetPixelColor(m_fiImage, x, y, &rgb);
}


