#ifndef _Q_IMAGE_H
#define _Q_IMAGE_H

class Image {
	public:
		Image();
		Image(const char *filePath);
		~Image();

		void setSize(int width, int height);
		void save();

		const int heightInBlocks();
		const int widthInBlocks();

		void setPixel(RGBQUAD &rgb, int x, int y);
		void getPixel(RGBQUAD &rgb, int x, int y);

		int randX();
		int randY();

		int width;
		int height;
		static int blockHeight;
		static int blockWidth;
		static int vertOverlapAmount;
		static int horizOverlapAmount;
	private:
		fipImage m_fiImage;
		const char *m_filePath;
};
