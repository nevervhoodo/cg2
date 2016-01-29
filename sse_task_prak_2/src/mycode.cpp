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
#include "EasyBMP.h"
#include "dop.h"

/*!
@file mycode.cpp
@brief The implemetation of the base part of the second mashgraph task and the mechanism of adding extra part
*/
#define DOP true ///<The macros indicating usage of the extra part of the mashgraph task

bool sseconstant = false; ///<Usage of SSE

/*!
@function Converts coloured BMP picture into the grey one. 
@param [in] m Pointer to the image 
@return Matrix with floats. Each float number is the grey variant of the pixel
*/
GImg makegray (BMP *m); 

/*!
@function Counts features for each cell of the image. 
* Sorts arguments of gradients and accumulate corresponding modules of gradients
@param [in] mod Reference to the cell of the matrix with modules of gradients
@param [in] arg Reference to the cell of the matrix with arguments of gradients
@return Vector with count features for the cell of the picture. 
*/
vector<float> cellhog (GImg &mod, GImg &arg);

/*!
 * @function Makes normalization of the vector with features of one cell. 
 * @param [in,out] hog Reference to the vector of features, considering the part to be normalized
 * @param [in] place The place in the hog where the normalization is to start 
*/
void normalization (vector <float>&hog, uint place);

/*!
 * @function Makes normalization of the vector with features of one cell using SSE
 * @param [in,out] hog Reference to the vector of features, considering the part to be normalized
 * @param [in] place The place in the hog where the normalization is to start 
*/
void ssenormalization (vector <float>&hog, uint place);

void checkconstants(bool sse)
{
	if (NARGS%4)
		throw "CHANGE NARGS! IT SHOUD BE DIVISIBLE BY 4\n";
	sseconstant = sse;
}

vector <float> myhog (BMP *m)
{
	GImg image;
	image = makegray(m);
	if (DOP)
	{
		setdopssefeature(sseconstant);
		vector <float> colors = colorfeatures(m);
		vector <float> fullhog = piramid (image);
		vector <float> unlinearhog;
		if (sseconstant)
			unlinearhog= sseunlinearsvmkernel(fullhog);
		else 	
			unlinearhog= unlinearsvmkernel(fullhog);
		unlinearhog.insert(unlinearhog.end(), colors.begin(), colors.end());
		return unlinearhog;	
	}
	return counthog(sobelfilter(image));
}

GImg makegray (BMP *m)
{
	uint rows, cols;
	rows = m->TellWidth();
	cols = m->TellHeight();
	GImg grayimg (rows,cols);
	RGBApixel pixel;
	for (uint i=0;i<rows;i++)
		for (uint j=0;j<cols;j++)
		{
			pixel = m->GetPixel(i,j);
			grayimg(i,j)=round(0.299*pixel.Red + 0.114*pixel.Blue + 0.587*pixel.Green);
		}
	
	return grayimg;
}

pair<GImg, GImg> sobelfilter(GImg& src)
{
	float xgrad;
	float ygrad;
	GImg modgrad(src.n_rows,src.n_cols);
	GImg arggrad(src.n_rows,src.n_cols);
	
	GImg extendimg = src.extra_borders(1, 1);
	for (uint i=0;i<src.n_rows;i++)
		for (uint j=0;j<src.n_cols;j++)
		{
			xgrad = extendimg(i,j)+ extendimg(i+2,j) +2*extendimg(i+1,j) 
				- 2*extendimg(i+1,j+2)-extendimg(i,j+2)-extendimg(i+2,j+2);
			ygrad = extendimg(i,j)+ extendimg(i,j+2) +2*extendimg(i,j+1) 
				- 2*extendimg(i+2,j+1)-extendimg(i+2,j)-extendimg(i+2,j+2);
			modgrad (i,j) = sqrt(xgrad*xgrad+ygrad*ygrad);
			arggrad (i,j) = atan2(ygrad,xgrad);
		}
	return make_pair(modgrad,arggrad);
}

pair<GImg, GImg> ssesobelfilter(GImg& src)
{
	float xgr;
	float ygr;
	GImg modgrad(src.n_rows,src.n_cols);
	GImg arggrad(src.n_rows,src.n_cols);
	
	__m128 up,down,xgrad, ygrad;
	__m128 upr,downr,right, upl, downl;
	GImg ext = src.extra_borders(1, 1);
	for (uint i=0;i<src.n_rows;i++)
	{

		uint j;
		for (j=0;j<=(src.n_cols-5);j+=4)
		{
			up = _mm_loadu_ps(&(ext(i,j+1)));
			xgrad = _mm_add_ps(up,up);
			upl = _mm_loadu_ps(&(ext(i,j)));
			xgrad = _mm_add_ps(xgrad,upl);
			downl = _mm_loadu_ps(&(ext(i+2,j)));
			xgrad = _mm_sub_ps(xgrad,downl);
			
			down = _mm_loadu_ps(&(ext(i+2,j+1)));
			xgrad = _mm_sub_ps(xgrad,down);
			xgrad = _mm_sub_ps(xgrad,down);
			upr = _mm_loadu_ps(&(ext(i,j+2)));
			xgrad = _mm_add_ps(xgrad,upr);
			downr = _mm_loadu_ps(&(ext(i+2,j+2)));

			xgrad = _mm_sub_ps(xgrad,downr);		
			right = _mm_loadu_ps(&(ext(i+1,j+2)));
			right = _mm_sub_ps(_mm_loadu_ps(&(ext(i+1,j))),right);
			right = _mm_add_ps(right,right);
			right = _mm_sub_ps(right,upr);
			right = _mm_sub_ps(right,downr);
			right = _mm_add_ps(right,upl);
			right = _mm_add_ps(right,downl);
					
			ygrad = xgrad;
			xgrad = right;
			
			for (uint p=0;p<=3;p++)
				arggrad(i,j+p) = atan2(ygrad[p],xgrad[p]);
			
			__m128 sq = _mm_sqrt_ps(_mm_add_ps
				(_mm_mul_ps(xgrad,xgrad),_mm_mul_ps(ygrad,ygrad)));
			_mm_storeu_ps(&(modgrad(i,j)),sq);
		}
		for (;j<src.n_cols;j++)
		{
			xgr = ext(i,j) + ext(i+2,j) +2*ext(i+1,j) 
				- 2*ext(i+1,j+2)-ext(i,j+2)-ext(i+2,j+2);
			ygr = ext(i,j)+ ext(i,j+2) +2*ext(i,j+1) 
				- 2*ext(i+2,j+1)-ext(i+2,j)-ext(i+2,j+2);
			modgrad (i,j) = sqrt(xgr*xgr+ygr*ygr);
			arggrad (i,j) = atan2(ygr,xgr);
		}
	}
	return make_pair(modgrad,arggrad);
}

vector <float> counthog(pair <GImg , GImg > got)
{
	GImg &mod = got.first;
	GImg &arg = got.second;
	vector<float> hog(PARTS*NARGS);
	for (uint i=0;i<NARGS*PARTS;i++)
		hog[i]=0;
	uint rows = mod.n_rows/YPARTS;
	uint cols = mod.n_cols/XPARTS;
	uint k = 0;
	uint n,m;
	n = 0;
	for (uint i=0;(i<mod.n_rows)&&(n<YPARTS);n++,i+=rows)
	{
		m = 0;
		for (uint j=0;(j<mod.n_cols)&&(m<XPARTS);m++,j+=cols,k+=NARGS)
		{
			uint n_rows = rows;
			uint n_cols = cols;	
			if (n==YPARTS-1)
				n_rows = mod.n_rows - i;
			if (m==XPARTS-1)
				n_cols = mod.n_cols - j;
			GImg argcell = arg.submatrix (i,j,n_rows,n_cols);
			GImg modcell = mod.submatrix (i,j,n_rows,n_cols);
			vector <float> subhog = cellhog(modcell,argcell);
			for (uint x=0;x<NARGS;x++)
				hog [x+k] = subhog[x];
			if (sseconstant)
				ssenormalization(hog,k);
			else
				normalization(hog,k);
		}
	}
	return hog;
}

vector<float> cellhog (GImg &mod, GImg &arg)
{
	uint x;
	vector <float> hog (NARGS);
	for (uint i=0;i<mod.n_rows;i++)
		for (uint j=0; j<mod.n_cols;j++)
		{
			x = (M_PI + arg(i,j))/2/M_PI*NARGS;
			hog[x%NARGS]+=mod(i,j);
		}
		
	return hog;
}

void normalization (vector <float>&hog, uint place)
{
	float acc = 0;
	for (uint t=0;(t<NARGS)&&(t<hog.size());t++)
		acc+=hog[place+t]*hog[place+t];
	acc = sqrt (acc);
	if (acc< PRECISION)
		for (uint t=0;t<NARGS;t++)
			hog[t+place] = 0.0;
	else
		for (uint t=0;t<NARGS;t++)
			hog[t+place] /= acc; 
}

void ssenormalization (vector <float>&hog, uint place)
{
	float acc = 0;
	uint size = NARGS/4;
	__m128 /*h[NARGS/4],*/ sum, temp, u;
	sum = _mm_setzero_ps();
	uint k=place;
	for (uint t=0;(t<size)&&(k<hog.size());t++,k+=4)
	{
		u = _mm_loadu_ps(&(hog[k]));
		temp = _mm_mul_ps(u,u);
		sum = _mm_add_ps(sum, temp);
	}
	sum = _mm_add_ps(sum, _mm_movehl_ps(sum, sum));
	sum = _mm_add_ss(sum, _mm_shuffle_ps(sum, sum, 1));
	acc = sqrt (sum[0]);
	sum = _mm_set1_ps(acc);
	k = place;
	if (acc< PRECISION)
		for (uint t=0;t<NARGS;t++)
			hog[t+place] = 0.0;
	else
		for (uint t=0;(t<size)&&(k<hog.size());t++,k+=4)
		{
			u = _mm_loadu_ps(&(hog[k]));
			u =_mm_div_ps(u,sum);
			_mm_store_ps(&(hog[k]),u);
		}
}
