#include <cstdlib>
#include <climits>
#include <cmath>
#include <iostream>
#include "FreeImage.h"
bool doingTransfer;
float alpha;
int blockHeight;
int blockWidth;
int numIterations;
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

void quilt(Image *target, Image *target_crsp, int rows, int cols, Image *source, Image *source_crsp) {
	for (int n = 0; n < numIterations; n++) {
		//alpha = 0.8 * (n / float(numIterations)) + .1;
		alpha = .1;
		const int numTrials = sqrt((source->width - blockWidth) * (source->height - blockHeight));
		std::cout << "quilting with " << numTrials << " trials" << std::endl; // %%%
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				int x, y;
				findOverlap(&x, &y, target, target_crsp, i, j, source, source_crsp, numTrials);
				int vertCut[blockHeight];
				int horizCut[blockWidth];
				findErrorSurface(vertCut, horizCut, target, i, j, source, x, y);
				pasteBlock(target, i, j, vertCut, horizCut, source, x, y);
			}
		}
		blockHeight /= 3; //hmmmmm
	}
}

int main(int argc, const char **argv) {
	if (argc == 5) {
		doingTransfer = true;
	}
	else {
		doingTransfer = false;
	}
	if (argc != 3 && argc != 5) {
		std::cerr << "usage: " << argv[0] << " <src> <dst> <src-crsp> <dst-crsp> " << std::endl;
		return 2;
	}

	blockHeight = 64;
	blockWidth = 64;
	numIterations = 1;
	vertOverlapAmount = 10;
	horizOverlapAmount = 10;
	FreeImage_Initialise(true);

	const int rows = 8;
	const int cols = 8;

	Image * const src = new Image();
	src->load(argv[1]);
	Image * const dst = allocate(rows, cols);
	srand(time(NULL));
	Image * const src_crsp = doingTransfer ? new Image() : NULL;
	Image * const dst_crsp = doingTransfer ? new Image() : NULL;
	if (doingTransfer) {
		src_crsp->load(argv[3]);
		dst_crsp->load(argv[4]);
	}
	quilt(dst, dst_crsp, rows, cols, src, src_crsp);
	dst->save(argv[2]);

	delete dst;
	delete src;
	delete dst_crsp;
	delete src_crsp;

	FreeImage_DeInitialise();
}
