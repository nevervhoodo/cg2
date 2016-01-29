#ifndef MY_CODE_H
#define MY_CODE_H

#include "EasyBMP.h"
#include "matrix.h"

#include <tuple>
#include <vector>
#define XPARTS 10
#define YPARTS 6
#define PARTS (XPARTS*YPARTS)
#define NARGS 16

typedef unsigned int uint;
typedef Matrix<float> GImg;

vector <float> myhog (BMP *m);

GImg makegray (BMP *m);

std::pair<GImg, GImg> sobelfilter(GImg& src);

std::vector <float> counthog(std::pair <GImg , GImg>);

vector<float> cellhog (GImg &mod, GImg &arg);

void normalization (vector <float>&hog, uint place);

std::tuple<float,float,float,float> takeneighs (GImg *src, uint i, uint j);

#endif
