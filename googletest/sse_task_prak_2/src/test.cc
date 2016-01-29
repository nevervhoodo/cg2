#include <gtest.h>

#include <cassert>
#include <iostream>
#include <emmintrin.h>
#include <cmath>

#include "EasyBMP.h"
#include "linear.h"
#include "matrix.h"
#include "Timer.h"

#include <fstream>
#include <xmmintrin.h>
#include <immintrin.h>

#define PRECISION 0.00001

typedef unsigned int uint;
typedef Matrix<float> GImg;

const char *file;

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

GImg sobelfilter(GImg& src)
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
		}
	return modgrad;
}

GImg ssesobelfilter(GImg& src)
{
	float xgr;
	float ygr;
	GImg modgrad(src.n_rows,src.n_cols);
	
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
		}
	}
	return modgrad;
}

TEST(SSE, NoSSE)
{
	BMP *in = new BMP;
	in->ReadFromFile(file);
	GImg img = makegray(in);
	Timer T;
	T.start();
	GImg withsse = ssesobelfilter(img);
	T.check();
	T.restart();
	GImg nosse = sobelfilter(img);
	T.check();
    for (uint i = 0; i < abs(in->TellWidth()); i++)
		for (uint j = 0; j< abs(in->TellHeight());j++)
		{
			EXPECT_LT(fabsf(withsse(i,j)- nosse(i,j)),PRECISION);
		}
}

int main(int argc, char *argv[])
{
	if (argc<1)
	{
		std::cout<<"Usage: need picture as input"<<std::endl;
		return -1;
	}
	file = argv[1];
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
