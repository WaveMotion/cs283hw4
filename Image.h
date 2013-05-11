#ifndef _Q_IMAGE_H
#define _Q_IMAGE_H

#include "FreeImage.h"


class Image {
	public:
		Image();
		Image(int width, int height);
		~Image();

		void setSize(int width, int height);

		void load(const char *filePath);
		void save(const char *filePath);

		void setPixel(RGBQUAD &rgb, int x, int y) const;
		void getPixel(RGBQUAD &rgb, int x, int y) const;

		int width;
		int height;
		//static int blockHeight = 64;
		//static int blockWidth = 64;
		//static int vertOverlapAmount = 8;
		//static int horizOverlapAmount = 8;
		
	private:
		FIBITMAP *m_fiImage;
};
#endif // _Q_IMAGE_H
