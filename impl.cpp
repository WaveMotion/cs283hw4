extern int blockWidth, blockHeight, horizOverlapAmount, vertOverlapAmount;

void getBlockOrigin(int *x, int *y, int i, int j) {
	*x = j * (blockWidth - horizOverlapAmount);
	*y = i * (blockHeight - vertOverlapAmount);
}

int copyPixel(Image *d, int dx, int dy, Image s, int sx, int sy) {
	RGBQUAD color;
	getPixel(color, s, sx, sy);
	setPixel(d, dx, dy, color);
}

int errorPixel(Image *a, int ax, int ay, Image b, int bx, int by) {
	RGBQUAD pa;
	RGBQUAD pb;
	getPixel(pa, a, ax, ay);
	getPixel(pb, b, bx, by)
	int dr = pa.r - pb.r;
	int dg = pa.g - pb.g;
	int db = pa.b - pb.b;
	return dr * dr + dg * dg + db * db;
}

int OVERLAP_THRESHOLD = 128;

void errorRect(int *error, int *threshold, const Image *a, int ax, int ay, Image b, int bx, int by, int width, int height) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			*error += errorPixel(a, ax + j, ay + i, b, bx + j, by + i);
		}
	}
	*threshold += OVERLAP_THRESHOLD * width * height;
}

bool overlap(const Image *target, int i, int j, const Image *source, int sx, int sy) {
	// just compare the total squared difference of all pixels to some threshold lol
	int tx, ty;
	getBlockOrigin(&tx, &ty, i, j);
	int error = 0;
	int threshold = 0;
	if (i) {
		errorRect(&error, &threshold, target, tx, ty, source, sx, sy, blockWidth, vertOverlapAmount);
		if (j) {
			errorRect(&error, &threshold, target, tx, ty + vertOverlapAmount, source, sx, sy + vertOverlapAmount, horizOverlapAmount, blockHeight - vertOverlapAmount);
		}
	} else {
		if (j) {
			errorRect(&error, &threshold, target, tx, ty, source, sx, sy, horizOverlapAmount, blockHeight);
		}
	}
	return error < threshold;
}

struct dpcell { int error, prev; };

void findErrorSurface(int *vertCut, int *horizCut, Image *target, int i, int j, Image *source, int sx, int sy) {
	int tx, ty;
	getBlockOrigin(&tx, &ty, i, j);
	dpcell vert[blockHeight][horizOverlapAmount];
	dpcell horiz[blockWidth][vertOverlapAmount];
	if (i) {
		// run dynamic programming for top edge (right to left)
		for (int i = blockHeight - 1; i >= 0; i--) {
			for (int j = 0; j < horizOverlapAmount; j++) {
				int error = errorPixel(target, tx + j, ty + i, source, sx + j, sx + i)
					if (i >= blockHeight - 1) {
						vert[i][j].error = error;
						vert[i][j].prev = j;
					} else {
						int min = vert[i + 1][j];
						int prev = j;
						if (i < horizOverlapAmount - 1 && vert[i + 1][j + 1].error < min) {
							min = vert[i + 1][j + 1];
							prev = j + 1;
						}
						if (i > 0 && vert[i + 1][j - 1] < min) {
							min = vert[i + 1][j - 1];
							prev = j - 1;
						}
						vert[i][j].error = min + error;
						vert[i][j].prev = prev;
					}
			}
		}
	}
	if (j) {
		// run dynamic programming for left edge (bottom to top)
		for (int j = blockWidth - 1; j >= 0; j--) {
			for (int i = 0; i < vertOverlapAmount; i++) {
				int error = errorPixel(target, tx + j, ty + i, source, sx + j, sx + i)
					if (j >= blockWidth - 1) {
						horiz[j][i].error = error;
						horiz[j][i].prev = i;
					} else {
						int min = horiz[j + 1][i];
						int prev = i;
						if (j < vertOverlapAmount - 1 && horiz[j + 1][i + 1].error < min) {
							min = horiz[j + 1][i + 1];
							prev = i + 1;
						}
						if (j > 0 && horiz[j + 1][i - 1] < min) {
							min = horiz[j + 1][i - 1];
							prev = i - 1;
						}
						horiz[j][i].error = min + error;
						horiz[j][i].prev = prev;
					}
			}
		}
	}
	int cx = 0;
	int cy = 0;
	if (i && j) {
		// find optimal corner
		int min = INT_MAX;
		for (int i = 0; i < vertOverlapAmount; i++) {
			for (int j = 0; j < horizOverlapAmount; j++) {
				int alt = vert[i][j].error + horiz[j][i].error;
				if (alt < min) {
					min = alt;
					cx = j;
					cy = i;
				}
			}
		}
	}
	if (i) {
		// populate top surface
		for (int i = 0; i < cy; i++) {
			vertCut[i] = cx;
		}
		for (int i = cy, j = cx; i < blockHeight; i++) {
			vertCut[i] = j;
			j = vert[i][j].prev;
		}
	} else {
		for (int i = 0; i < blockHeight; i++) {
			vertCut[i] = 0;
		}
	}
	if (j) {
		// populate left surface
		for (int j = 0; j < cx; j++) {
			horizCut[j] = cy;
		}
		for (int j = cy, i = cx; j < blockWidth; j++) {
			horizCut[j] = i;
			i = horiz[j][i].prev;
		}
	} else {
		for (int j = 0; j < blockWidth; j++) {
			horizCut[j] = 0;
		}
	}
}

bool inCut(int *vertCut, int *horizCut, int i, int j) {
	return j >= vertCut[j] && i >= horizCut[i];
}

void pasteBlock(Image *target, int i, int j, int *vertCut, int *horizCut, Image *source, int sx, int sy) {
	int tx, ty;
	getBlockOrigin(&tx, &ty, i, j);
	for (int i = 0; i < blockHeight; i++) {
		for (int j = 0; j < blockWidth; j++) {
			if (!inCut(vertCut, horizCut, i, j)) continue;
			copyPixel(target, tx + j, ty + i, source, sx + j, sy + i);
		}
	}
}
