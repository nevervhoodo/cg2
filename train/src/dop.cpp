#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <iostream>
#include <tuple>
#include <cmath>

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::pair;
using std::make_pair;
using std::make_tuple;
using std::tuple;
using std::tie;
using std::cout;
using std::cerr;
using std::endl;

#include "matrix.h"
#include "mycode.h"
#include "dop.h"

#define NPARTS 2
#define PIRAMIDPARTS (pow(NPARTS,2)+1)
#define SUBHOGSIZE (NARGS*PARTS)

#define NCOLORS 3
#define XYCOLORS 8 
#define NCOLORCELLS (pow(XYCOLORS,2)+1)

#define L_CONST 0.5
#define N_CONST 1

vector <float> piramid(GImg &img)
{
	vector<float> hog(HOGSIZE);
	uint rows = img.n_rows/NPARTS;
	uint cols = img.n_cols/NPARTS;
	uint k = SUBHOGSIZE;
	vector <float> subhog = counthog(sobelfilter(img));
		for (uint x=0;x<SUBHOGSIZE;x++)
			hog [x] = subhog[x];
	uint n,m;
	n = 0;
	for (uint i=0;(i<img.n_rows)&&(n<NPARTS);n++,i+=rows)
	{
		m = 0;
		for (uint j=0;(j<img.n_cols)&&(m<NPARTS);m++,j+=cols,k+=SUBHOGSIZE)
		{
			uint n_rows = rows;
			uint n_cols = cols;
			if (n==NPARTS-1)
				n_rows = img.n_rows - i;
			if (m==NPARTS-1)
				n_cols = img.n_cols - j;
			GImg subimg = img.submatrix (i,j,n_rows,n_cols);
			subhog = counthog(sobelfilter(subimg));
			for (uint x=0;x<SUBHOGSIZE;x++)
				hog [x+k] = subhog[x];
		}
	}
	return hog;
}

vector <float> colorfeatures(BMP *bmp)
{
	uint bmprows, bmpcols;
	bmprows = bmp->TellWidth();
	bmpcols = bmp->TellHeight();
	vector<float> colors;
	uint rows = bmprows/XYCOLORS;
	uint cols = bmpcols/XYCOLORS;
	float r,g,b;
	uint n,m;
	n = 0;
	for (uint i=0;(i<bmprows)&&(n<XYCOLORS);n++,i+=rows)
	{
		m = 0;
		for (uint j=0;(j<bmpcols)&&(m<XYCOLORS);m++,j+=cols)
		{
			uint n_rows = rows;
			uint n_cols = cols;
			if (n==XYCOLORS-1)
				n_rows = bmprows - i;
			if (m==XYCOLORS-1)
				n_cols = bmpcols - j;
			tie (r,g,b) = countcolorfeatures(bmp,i,j,n_rows,n_cols);
			colors.push_back(r);
			colors.push_back(g);
			colors.push_back(b);
		}
	}
	return colors;
}

tuple <float,float,float> countcolorfeatures (BMP *bmp, uint y,uint x, 
	uint n_rows,uint n_cols)
{
	RGBApixel pixel;
	vector <float> colors;
	uint npix = n_rows*n_cols*255;
	float r,g,b;
	r=g=b=0.0;
	for (uint i=0;i<n_rows;i++)
		for (uint j=0;j<n_cols;j++)
		{
			pixel = bmp->GetPixel(i+y,j+x);
			r+=pixel.Red;
			g+=pixel.Green;
			b+=pixel.Blue;
		}
	r/=npix;
	g/=npix;
	b/=npix;
	return make_tuple(r,g,b);
}

vector <float> unlinearsvmkernel(vector<float> &srchog)
{
	vector <float> hog;
	float x;
	float lambda;
	for (uint i=0;i<srchog.size();i++)
	{
		x = srchog[i];
		for (int n=-N_CONST;n<=N_CONST;n++)
		{
			lambda = L_CONST*n;
			if (x<0.00001)
			{
				hog.push_back(0.0);
				hog.push_back(0.0);
			}
			else
			{
				hog.push_back(cos(-(lambda*log(x)))*sqrt(x*mysech(M_PI*lambda)));
				hog.push_back(sin(-(lambda*log(x)))*sqrt(x*mysech(M_PI*lambda)));
			}
		}
	} 
	return hog;
}

float mysech (float x)
{
	return 2/(exp(x)+exp(-x));
}

