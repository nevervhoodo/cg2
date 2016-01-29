/**
 * @file dop.h
 * @brief The describtion of functions and constants of extra part of the mashgraph task
 */

#ifndef DOP_H
#define DOP_H

#include "matrix.h"

#include <tuple>
#include <vector>
#include <xmmintrin.h>
#include <immintrin.h>

#define HOGSIZE (PIRAMIDPARTS*NARGS*PARTS) ///<Size of the extended vector with features 
#define COLORHOGSIZE (NCOLORCELLS*NCOLORS) ///<Size of the vector with color features

/**
@function Sets the flag ssedopconstant.
@param [in] sse carries the flag value.
*/
void setdopssefeature(bool sse);
/**
@function Counts features five times. The first one for the entire picture
* Then picture is devided into four cells of the same size. 
* The next four times function counts features for four cells of the picture. 
@param [in] img Reference to the grey image which is the matrix of floats
@return Returns the extended vector with features. 
* Made by concatination of five vectors counted by the function
*/
vector <float> piramid(GImg &img);

/**
 * @function Counts color features of the image
 * @param [in] bmp Pointer to the processed image
 * @return Vector with features
 */ 
vector <float> colorfeatures(BMP *bmp);

/**
 * @function Recounts vector with features for the unlinear SVM kernel.
 * Just replaces each float feature with the several another floats got after using some strange formulas.
 * @param [in] srchog Vector of source features.
 * @return Vector of features ready for unlinear SVM kernel.
 */
vector <float> unlinearsvmkernel(vector<float> &srchog);

/**
 * @function Recounts vector with features for the unlinear SVM kernel using SSE.
 * Just replaces each float feature with the several another floats got after using some strange formulas.
 * @param [in] srchog Vector of source features.
 * @return Vector of features ready for unlinear SVM kernel.
 */
vector <float> sseunlinearsvmkernel(vector<float> &srchog);
#endif
