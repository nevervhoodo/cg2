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
#include "EasyBMP.h"
#include "dop.h"
//#include "matrix.hpp"

#define DEBUG false
#define DEBMINUS false
#define DFILTER false
#define DHOG false
#define DOP true

vector <float> myhog (BMP *m)
{
	GImg image = makegray(m);
	if (DOP)
	{
		vector <float> colors = colorfeatures(m);
		vector <float> fullhog = piramid (image);
		vector <float> unlinearhog = unlinearsvmkernel(fullhog);
		//fullhog.insert(fullhog.end(), colors.begin(), colors.end());
		unlinearhog.insert(unlinearhog.end(), colors.begin(), colors.end());
		//return fullhog;
		return unlinearhog;
		//return unlinearsvmkernel(fullhog);		
	}
	return counthog(sobelfilter(image));
}

GImg makegray (BMP *m)
{
	uint rows, cols;
	rows = m->TellWidth();
	cols = m->TellHeight();
	if (DEBUG)
		cout<<"new img: "<<rows<<"x"<<cols<<endl;
	GImg grayimg (rows,cols);
	RGBApixel pixel;
	for (uint i=0;i<rows;i++)
		for (uint j=0;j<cols;j++)
		{
			pixel = m->GetPixel(i,j);
			grayimg(i,j)=0.299*pixel.Red + 0.114*pixel.Blue + 0.587*pixel.Green;
			if(DEBMINUS)
				if (grayimg(i,j)<-0.00001)
					cout<<"errr"<<endl;
		}
	
	return grayimg;
}

pair<GImg, GImg> sobelfilter(GImg& src)
{
	if (DFILTER)
		cout<<"sobelfilter"<<endl;
	GImg xgrad(src.n_rows,src.n_cols);
	GImg ygrad(src.n_rows,src.n_cols);
	GImg modgrad(src.n_rows,src.n_cols);
	GImg arggrad(src.n_rows,src.n_cols);
	float up, down, left, right;
	for (uint i=0;i<src.n_rows;i++)
		for (uint j=0;j<src.n_cols;j++)
		{
			tie (up, down, left, right) = takeneighs(&src,i,j);
			xgrad (i,j) = right - left;
			ygrad (i,j) = down - up;
			modgrad (i,j) = sqrt(xgrad(i,j)*xgrad(i,j)+ygrad(i,j)*ygrad(i,j));
			arggrad (i,j) = atan2(ygrad(i,j),xgrad(i,j));
		}
	if (DFILTER)
		cout<<"filter ended"<<endl;
	return make_pair(modgrad,arggrad);
}

vector <float> counthog(pair <GImg , GImg > got)
{
	if (DHOG)
		cout<<"counthog"<<endl;
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
			if (DHOG)
				cout<<"new cell "<<i<<" "<<j<<endl;	
			if (n==YPARTS-1)
				n_rows = mod.n_rows - i;
			if (m==XPARTS-1)
				n_cols = mod.n_cols - j;
			GImg argcell = arg.submatrix (i,j,n_rows,n_cols);
			GImg modcell = mod.submatrix (i,j,n_rows,n_cols);
			if (DHOG)
				cout<<"count hog in cell"<<endl;
			vector <float> subhog = cellhog(modcell,argcell);
			for (uint x=0;x<NARGS;x++)
				hog [x+k] = subhog[x];
			if (DHOG)
				cout<<"normalization"<<endl;
			normalization(hog,k);
			if (DHOG)
				cout<<"end of cell"<<endl;
		}
	}
	if (DHOG)
		cout<<"end of counthog"<<endl;
	if (DEBMINUS)	
		for (uint i=0;i<NARGS*PARTS;i++)
			if (hog[i]<-0.00001)
				cout<<"ups"<<endl;
	return hog;
}

vector<float> cellhog (GImg &mod, GImg &arg)
{
	uint x;
	vector <float> hog (NARGS);
	for (uint i=0;i<mod.n_rows;i++)
		for (uint j=0; j<mod.n_cols;j++)
		{
			if (arg(i,j)>0.000001)
				x = arg(i,j)/2/M_PI*NARGS;
			else
				x = (2*M_PI + arg(i,j))/2/M_PI*NARGS;
			hog[x]+=mod(i,j);
		}
	return hog;
}

void normalization (vector <float>&hog, uint place)
{
	float acc = 0;
	for (uint t=0;(t<NARGS)&&(t<hog.size());t++)
		acc+=hog[place+t]*hog[place+t];
	acc = sqrt (acc);
	if (acc< 0.000001)
		for (uint t=0;t<NARGS;t++)
			hog[t+place] = 0.0;
	else
		for (uint t=0;t<NARGS;t++)
			hog[t+place] /= acc; 
}

tuple<float,float,float,float> takeneighs (GImg *src, uint i, uint j)
{
	float up, down, left, right;
	if (i==0)
		up = (*src) (i,j);
	else
		up = (*src)(i-1,j);	
	if (i==(src->n_rows-1))
		down = (*src) (i,j);
	else
		down = (*src)(i+1,j);
	if (j==0)
		left = (*src) (i,j);
	else
		left = (*src)(i,j-1);
	if (j==(src->n_cols-1))
		right = (*src) (i,j);
	else
		right = (*src) (i,j+1);
	return make_tuple(up,down,left,right);
}
