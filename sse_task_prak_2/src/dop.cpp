#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <iostream>
#include <tuple>
#include <cmath>
#include <xmmintrin.h>
#include <immintrin.h>

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

/**
 * @file dop.cpp
 * @brief The implemetation of the extra part of the second mashgraph task
 */

#define NPARTS 2 ///< The number of vertical and horizontal cells of the picture for piramid function
#define PIRAMIDPARTS (pow(NPARTS,2)+1) ///<The total number of vectors counted in piramid function
#define SUBHOGSIZE (NARGS*PARTS) ///<The size of each vector counted in piramid function

#define NCOLORS 3 ///< The number of colors channels of the picture
#define XYCOLORS 8 ///< The number of vertical and horizontal cells of the picture for colorfeature function
#define NCOLORCELLS (pow(XYCOLORS,2)+1) ///<The total number of cells in colorfeature function

#define L_CONST 0.5 ///< One constant for the unlinearsvmkernel. Can be from 0.25 to 0.50 
#define N_CONST 2 ///< Anothe constant for the unlinearsvmkernel. Can be 1 or 2

bool ssedopconstant = false; ///<Usage of SSE in extra part of the task
/**
 * @function Counts color features for each cell of the picture
 * @param [in] img Pointer to the image 
 * @param [in] y Y coordinate of the cell to be processed
 * @param [in] x X coordinate of the cell to be processed
 * @param [in] n_rows Horizontal size of the cell
 * @param [in] n_colss Vertical size of the cell
 * @return Tuple of the R,G,B features of the cell
 */
tuple <float,float,float> countcolorfeatures (BMP *img, uint y,uint x, 
	uint n_rows,uint n_cols);
	
/**
 * @function Counts sech
 * @param [in] Input float
 * @return The result
 */
float mysech (float x);

/**
 * @function Counts sech for SSE variable
 * @param [in] Input SSE float variable
 * @return The result is the same type
 */
__m128 ssemysech (__m128 x);	

void setdopssefeature(bool sse)
{
	ssedopconstant = sse;
}

vector <float> piramid(GImg &img)
{
	vector<float> hog(HOGSIZE);
	uint rows = img.n_rows/NPARTS;
	uint cols = img.n_cols/NPARTS;
	uint k = SUBHOGSIZE;
	vector <float> subhog;
	if (ssedopconstant)
		subhog = counthog(ssesobelfilter(img));
	else 
		subhog = counthog(sobelfilter(img));

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
			if (ssedopconstant)
				subhog = counthog(ssesobelfilter(subimg));
			else
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

vector <float> sseunlinearsvmkernel(vector<float> &srchog)
{
	vector <float> hog;
	float w;
	__m128 l1, l2, x, tmp1, tmp2;
	if (N_CONST==1)
		l1 = _mm_setr_ps(-L_CONST,L_CONST,0,0);
	else
	if (N_CONST==2)
		l1 = _mm_setr_ps(-2*L_CONST,-L_CONST,L_CONST,2*L_CONST);
	else
		cout<<"PUT ANOTHER N_CONST"<<endl;
	l2 = _mm_set1_ps(M_PI);
	l2 = _mm_mul_ps(l2,l1);
	l2 = ssemysech(l2);
	for (uint i=0;i<srchog.size();i++)
	{
		w = srchog[i];
		x = _mm_set1_ps(w);
		if (w<0.00001)
			for (uint p=0;p<N_CONST*4+2;p++)
				hog.push_back(0.0);
		else
		{
			tmp2 = _mm_mul_ps(l1,_mm_set1_ps(log(w)));
			tmp1 = _mm_mul_ps(l2,x);
			tmp1 = _mm_sqrt_ps(tmp1);
			float tmp [N_CONST*4];
			for (uint p=0;p<N_CONST*2;p++)
			{
				if (p==N_CONST)
				{
					hog.push_back(sqrt(w));
					hog.push_back(0.0);
				}
				tmp[p*2] = cos(-tmp2[p])*tmp1[p];

				hog.push_back(tmp[2*p]);
				tmp[p*2+1] = sin(-tmp2[p])*tmp1[p];

				hog.push_back(tmp[2*p+1]);

			}
		}

	} 
	return hog;
}

float mysech (float x)
{
	return 2/(exp(x)+exp(-x));
}

__m128 ssemysech (__m128 x)
{
	__m128 res;
	for (uint p=0;p<=3;p++)
		res[p] = 2/(exp(x[p])+exp(-x[p]));
	return res;
}

