#include <random>
#include <iostream>
#include "FreeImage.h"
#include "Image.h"

Image::Image() {
	m_filePath = "";
}

Image::Image(const char *filePath) {
	m_filePath = filePath;
	m_fiImage = fipImage();
	bool loadSuccess = m_fiImage.load(filePath);
	if (!loadSuccess) { 
		cerr << "Loading file " << filePath << " unsuccessful" << std::endl;
		exit(1);
	}
	width = m_fiImage.getWidth();
	height = m_fiImage.getHeight();
}

Image::~Image() {
	m_fiImage.clear();
}

Image::save() {
	if (!m_fiImage.save(m_filePath)) {
		cerr << "Saving file " << m_filePath << " unsuccessful" << std::endl;
		exit(1);
	}
}

const int Image::heightInBlocks() {
	return width / blockHeight;
}

const int Image::widthInBlocks() {
	return height / blockWidth;
}

void Image::getPixel(RGBQUAD &rgb, int x, int y) {
	m_fiImage.getPixelColor(x, y, &rgb);
}

void Image::setPixel(RGBQUAD &rgb, int x, int y) {
	m_fiImage.setPixelColor(x, y, &rgb);
}

int Image::randX() {



