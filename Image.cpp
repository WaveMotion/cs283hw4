#include <cstdlib>
#include <iostream>
#include "FreeImage.h"
#include "Image.h"

Image::Image() :
	width(0),
	height(0),
	m_fiImage(NULL)
{
}

Image::Image(int _width, int _height) :
	width(_width),
	height(_height),
	m_fiImage(FreeImage_Allocate(width, height, 24))
{
}

Image::~Image() {
	if (m_fiImage) {
		FreeImage_Unload(m_fiImage);
		m_fiImage = NULL;
	}
}

void Image::load(const char *filePath) {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	fif = FreeImage_GetFileType(filePath, 0);
	if (fif == FIF_UNKNOWN) {
		std::cerr << "unknown source image type" << std::endl;
		exit(1);
	}
	m_fiImage = FreeImage_Load(fif, filePath, 0);
	if (!m_fiImage) {
		std::cerr << "unsuccessful file load" << std::endl;
		exit(1);
	}
	width = FreeImage_GetWidth(m_fiImage);
	height = FreeImage_GetHeight(m_fiImage);
}

void Image::save(const char *filePath) const {
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


