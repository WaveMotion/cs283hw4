
Image *source;
Image *target;
int blockHeight, blockWidth;


void quilt(Image source, Image target) {
	int targetHeight = target->heightInBlocks();
	int targetWidth = target->widthInBlocks();
	for (int i = 0; i < targetHeight; i++) {
		for (int j = 0; j < targetWidth; j++) {
			int x, y;
			do {
				x = source.randX();
				y = source.randY();
			} while (!overlap(target, i, j, source, x, y));
			int vertCut[blockHeight];
			int horizCut[blockWidth];
			findErrorSurface(vertCut, horizCut, target, i, j, source, x, y);
			pasteBlock(target, i, j, vertCut, horizCut, source, x, y);
		}
	}
}
