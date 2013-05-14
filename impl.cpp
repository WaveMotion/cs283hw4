void computePixelSize(int *width, int *height, int rows, int cols) {
	*width = cols * (blockWidth - horizOverlapAmount) + horizOverlapAmount;
	*height = rows * (blockHeight - vertOverlapAmount) + vertOverlapAmount;
}

int randBlock(int *x, int *y, const Image *i) {
	*x = rand() % (i->width - blockWidth);
	*y = rand() % (i->height - blockHeight);
}

void getBlockOrigin(int *x, int *y, int i, int j) {
	*x = j * (blockWidth - horizOverlapAmount);
	*y = i * (blockHeight - vertOverlapAmount);
}

int copyPixel(const Image *d, int dx, int dy, const Image *s, int sx, int sy) {
	RGBQUAD color;
	s->getPixel(color, sx, sy);
	d->setPixel(color, dx, dy);
}

int errorPixel(const Image *a, int ax, int ay, const Image *b, int bx, int by) {
	RGBQUAD pa;
	RGBQUAD pb;
	a->getPixel(pa, ax, ay);
	b->getPixel(pb, bx, by);
	int dr = pa.rgbRed - pb.rgbRed;
	int dg = pa.rgbGreen - pb.rgbGreen;
	int db = pa.rgbBlue - pb.rgbBlue;
	return dr * dr + dg * dg + db * db;
}

void errorRect(int *error, const Image *a, int ax, int ay, const Image *b, int bx, int by, int width, int height) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			*error += errorPixel(a, ax + j, ay + i, b, bx + j, by + i);
		}
	}
}

int overlap(const Image *target, int tx, int ty, const Image *source, int sx, int sy) {
	int error = 0;
	if (ty) {
		errorRect(&error, target, tx, ty, source, sx, sy, blockWidth, vertOverlapAmount);
		if (tx) {
			errorRect(&error, target, tx, ty + vertOverlapAmount, source, sx, sy + vertOverlapAmount, horizOverlapAmount, blockHeight - vertOverlapAmount);
		}
	} else {
		if (tx) {
			errorRect(&error, target, tx, ty, source, sx, sy, horizOverlapAmount, blockHeight);
		}
	}
	return error;
}

void findOverlap(int *sx, int *sy, const Image *target, const Image *target_crsp, int ti, int tj, const Image *source, const Image *source_crsp, int count) {
	int tx, ty;
	getBlockOrigin(&tx, &ty, ti, tj);
	int min = INT_MAX;
	for (int c = 0; c < count; c++) {
		int x, y;
		randBlock(&x, &y, source);
		int overlapError = overlap(target, tx, ty, source, x, y);
		int error;
		if (doingTransfer) {
			int correspondenceError = 0;
			errorRect(&correspondenceError, target_crsp, tx, ty, source_crsp, x, y, blockWidth, blockHeight);
			error = alpha * overlapError + (1 - alpha) * correspondenceError;
		}
		else {
			error = overlapError;
		}
		if (error < min) {
			min = error;
			*sx = x;
			*sy = y;
		}
		// std::cout << "overlap " << c << ": " << error << "/" << min << std::endl; // %%%
	}
}

struct dpcell { int error, prev; };

void findErrorSurface(int *vertCut, int *horizCut, const Image *target, const int ti, const int tj, const Image *source, int sx, int sy) {
	int tx, ty;
	getBlockOrigin(&tx, &ty, ti, tj);
	dpcell vert[blockHeight][horizOverlapAmount];
	dpcell horiz[blockWidth][vertOverlapAmount];
	if (ti) {
		// run dynamic programming for top edge (right to left)
		for (int j = blockWidth - 1; j >= 0; j--) {
			for (int i = 0; i < vertOverlapAmount; i++) {
				int error = errorPixel(target, tx + j, ty + i, source, sx + j, sy + i);
				if (j >= blockWidth - 1) {
					horiz[j][i].error = error;
					horiz[j][i].prev = i;
				} else {
					int min = horiz[j + 1][i].error;
					int prev = i;
					if (i < vertOverlapAmount - 1 && horiz[j + 1][i + 1].error < min) {
						min = horiz[j + 1][i + 1].error;
						prev = i + 1;
					}
					if (i > 0 && horiz[j + 1][i - 1].error < min) {
						min = horiz[j + 1][i - 1].error;
						prev = i - 1;
					}
					horiz[j][i].error = min + error;
					horiz[j][i].prev = prev;
				}
			}
		}
	}
	if (tj) {
		// run dynamic programming for left edge (bottom to top)
		for (int i = blockHeight - 1; i >= 0; i--) {
			for (int j = 0; j < horizOverlapAmount; j++) {
				int error = errorPixel(target, tx + j, ty + i, source, sx + j, sy + i);
				if (i >= blockHeight - 1) {
					vert[i][j].error = error;
					vert[i][j].prev = j;
				} else {
					int min = vert[i + 1][j].error;
					int prev = j;
					if (j < horizOverlapAmount - 1 && vert[i + 1][j + 1].error < min) {
						min = vert[i + 1][j + 1].error;
						prev = j + 1;
					}
					if (j > 0 && vert[i + 1][j - 1].error < min) {
						min = vert[i + 1][j - 1].error;
						prev = j - 1;
					}
					vert[i][j].error = min + error;
					vert[i][j].prev = prev;
				}
			}
		}
	}
	int cx = 0;
	int cy = 0;
	if (ti && tj) {
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
	if (ti) {
		// populate top surface
		for (int j = 0; j < cx; j++) {
			horizCut[j] = cy;
		}
		for (int j = cx, i = cy; j < blockWidth; j++) {
			horizCut[j] = i;
			i = horiz[j][i].prev;
		}
	} else {
		for (int j = 0; j < blockWidth; j++) {
			horizCut[j] = 0;
		}
	}
	if (tj) {
		// populate left surface
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
}

bool inCut(int *vertCut, int *horizCut, int i, int j) {
	return j >= vertCut[i] && i >= horizCut[j];
}

void pasteBlock(const Image *target, int ti, int tj, int *vertCut, int *horizCut, const Image *source, int sx, int sy) {
	int tx, ty;
	RGBQUAD fill = {rand(), rand(), rand(), 0};
	getBlockOrigin(&tx, &ty, ti, tj);
	for (int i = 0; i < blockHeight; i++) {
		for (int j = 0; j < blockWidth; j++) {
			if (!inCut(vertCut, horizCut, i, j)) continue;
			copyPixel(target, tx + j, ty + i, source, sx + j, sy + i);
			// target->setPixel(fill, tx + j, ty + i); // %%%
		}
	}
	// RGBQUAD edge = {fill.rgbBlue / 2, fill.rgbGreen / 2, fill.rgbRed / 2, 0}; // %%%
	// for (int i = 0; i < blockHeight; i++) {
	// 	target->setPixel(edge, tx + vertCut[i], ty + i);
	// }
	// for (int j = 0; j < blockWidth; j++) {
	// 	target->setPixel(edge, tx + j, ty + horizCut[j]);
	// }
}
