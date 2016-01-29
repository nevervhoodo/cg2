#ifndef DOP_H
#define DOP_H

#include "matrix.h"

#include <tuple>
#include <vector>

#define HOGSIZE (PIRAMIDPARTS*NARGS*PARTS)
#define COLORHOGSIZE (NCOLORCELLS*NCOLORS)

vector <float> piramid(GImg &img);
vector <float> colorfeatures(BMP *bmp);
tuple <float,float,float> countcolorfeatures (BMP *img, uint y,uint x, 
	uint n_rows,uint n_cols);
vector <float> unlinearsvmkernel(vector<float> &srchog);
float mysech (float x);

#endif
