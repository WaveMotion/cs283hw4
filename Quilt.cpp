#include <cstdlib>
#include <climits>
#include <cmath>
#include <iostream>
#include "FreeImage.h"
int blockHeight;
int blockWidth;
int vertOverlapAmount;
int horizOverlapAmount;

#include "Image.h"

//int blockHeight = Image::blockHeight;
//int blockWidth = Image::blockWidth;
//int vertOverlapAmount = Image::vertOverlapAmount;
//int horizOverlapAmount = Image::horizOverlapAmount;

#include "impl.cpp"


Image *allocate(int rows, int cols) {
	int width, height;
	computePixelSize(&width, &height, rows, cols);
	return new Image(width, height);
}

void quilt(Image *target, int rows, int cols, Image *source) {
	const int numTrials = sqrt((source->width - blockWidth) * (source->height - blockHeight));
	std::cout << "quilting with " << numTrials << " trials" << std::endl; // %%%
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			int x, y;
			findOverlap(&x, &y, target, i, j, source, numTrials);
			int vertCut[blockHeight];
			int horizCut[blockWidth];
			findErrorSurface(vertCut, horizCut, target, i, j, source, x, y);
			pasteBlock(target, i, j, vertCut, horizCut, source, x, y);
		}
	}
}

int main(int argc, const char **argv) {
	if (argc < 3) {
		std::cerr << "usage: " << argv[0] << " <src> <dst>" << std::endl;
		return 2;
	}

	blockHeight = 64;
	blockWidth = 64;
	vertOverlapAmount = 10;
	horizOverlapAmount = 10;
	FreeImage_Initialise(true);

	const int rows = 8;
	const int cols = 8;

	Image * const src = new Image();
	src->load(argv[1]);
	Image * const dst = allocate(rows, cols);
	srand(time(NULL));
	quilt(dst, rows, cols, src);
	dst->save(argv[2]);

	delete dst;
	delete src;

	FreeImage_DeInitialise();
}
