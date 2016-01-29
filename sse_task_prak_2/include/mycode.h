/*!
@file mycode.h
@brief The describtion of functions and constants of base part 
*/
#ifndef MY_CODE_H
#define MY_CODE_H

#include "EasyBMP.h"
#include "matrix.h"

#include <tuple>
#include <vector>
#include <xmmintrin.h>
#include <immintrin.h>


#define XPARTS 10 ///<Each picture is devided into XPARTS horizontal parts for counting feature vector
#define YPARTS 6 ///<Each picture is devided into YPARTS vertical parts for counting feature vector
#define PARTS (XPARTS*YPARTS) ///<Number of parts for counting features
#define NARGS 16 ///<Number of sectors of the circle to sort argument values of gradient function
#define PRECISION 0.00001 ///<The precision of countings

typedef unsigned int uint;
typedef Matrix<float> GImg;

/**
@function Decides if the constants are correct and sets the flag sseconstant.
@param [in] sse carries the flag value 
*/
void checkconstants(bool sse);
/**
@function Rules the process of counting features. Make all working step by step.
@param [in] m Pointer to the image we got
@return Returns ready vector of features  
*/
vector <float> myhog (BMP *m);
/**
@function Rules the process of fomation of counting gradient and converting info into features
@param [in] got Pair with two Matrixes with floats. 
* The first one is the matrix of modules of gradient of pixels.
* The second one is the matrix of arguments of gradient of pixels.
@return Returns ready vector of base features
*/
std::vector <float> counthog(std::pair <GImg , GImg> got);
/**
@function Counts gradient
@param [in] src Reference to the matrix of floats made by convering coloured image into grey one. 
@return Returns pair of matrixes. 
* The first one is the matrix of modules of gradient of pixels.
* The second one is the matrix of arguments of gradient of pixels.
*/
std::pair<GImg, GImg> sobelfilter(GImg& src);
/**
@function Counts gradient using SSE
@param [in] src Reference to the matrix of floats made by convering coloured image into grey one. 
@return Returns pair of matrixes. 
* The first one is the matrix of modules of gradient of pixels.
* The second one is the matrix of arguments of gradient of pixels.
*/
std::pair<GImg, GImg> ssesobelfilter(GImg& src);

#endif
