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


void quilt(Image *source, Image *target) {
	int targetHeight = getHeightInBlocks(target);
	int targetWidth = getWidthInBlocks(target);
	for (int i = 0; i < targetHeight; i++) {
		for (int j = 0; j < targetWidth; j++) {
			int x, y;
			do {
				x = randX(source);
				y = randY(source);
			} while (!overlap(target, i, j, source, x, y));
			int vertCut[blockHeight];
			int horizCut[blockWidth];
			findErrorSurface(vertCut, horizCut, target, i, j, source, x, y);
			pasteBlock(target, i, j, vertCut, horizCut, source, x, y);
		}
	}
}

int main(int argc, const char **argv) {
	blockHeight = 64;
	blockWidth = 64;
	vertOverlapAmount = 8;
	horizOverlapAmount = 8;
	FreeImage_Initialise(true);
	FreeImage_DeInitialise();
}
