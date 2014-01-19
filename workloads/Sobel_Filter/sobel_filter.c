/* sobel.c */

//==============================================================================
//Copyright (c) 2013 Intel Corporation.
//All rights reserved.
//==============================================================================
//
//Developed by:
//
//    Accelerator Architecture Lab, Intel Labs
//
//    Intel Corporation
//
//    http://www.intel.com
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of
//this software and associated documentation files (the "Software"), to deal with
//the Software without restriction, including without limitation the rights to
//use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
//of the Software, and to permit persons to whom the Software is furnished to do
//so, subject to the following conditions:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimers.
//
//    * Redistributions in binary form must reproduce the above copyright notice,
//      this list of conditions and the following disclaimers in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the names of the LLVM Team, University of Illinois at
//      Urbana-Champaign, nor the names of its contributors may be used to
//      endorse or promote products derived from this Software without specific
//      prior written permission.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
//FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
//CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE
//SOFTWARE.
//==============================================================================
//Contact: Asit K Mishra (asit.k.mishra@intel.com)

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include "mypgm.h"

#define AXC
#ifdef AXC
void kernel_filter(unsigned char in1, unsigned char in2, unsigned char in3,  unsigned char in4,  unsigned char in5,  unsigned char in6,  unsigned char in7,  unsigned char in8,  unsigned char in9, unsigned char image_window[][3], unsigned char *out)
#else
unsigned char kernel_filter(unsigned char image_window[][3])
#endif
{
  
  /* Definition of Sobel filter in horizontal direction */
  int weight_Gx[3][3] = {{ -1,  0,  1 },
		                 { -2,  0,  2 },
		                 { -1,  0,  1 }};

  /* Definition of Sobel filter in vertical   direction */
  int weight_Gy[3][3] = {{  1,  2,  1 },
		                 {  0,  0,  0 },
		                 { -1, -2, -1 }};
  
  double pixel_value_X;
  double pixel_value_Y;
  double SUM; 
  int i, j;  /* Loop variable */

  /* Generation of image2 after linear transformtion */
  pixel_value_X = 0.0;
  pixel_value_Y = 0.0;
  SUM           = 0.0;
  
  for (j = 0; j <= 2; j++) {
    for (i = 0; i <= 2; i++) {
    pixel_value_X += weight_Gx[j][i] * image_window[j][i]; //X-dir
    pixel_value_Y += weight_Gy[j][i] * image_window[j][i]; //Y-dir
    }
  }
  
  SUM = sqrt( pow(pixel_value_X,2) + pow(pixel_value_Y,2) );
  if(SUM>255) SUM = 255;
  if(SUM<0)   SUM = 0;
  //image2[y][x] = (unsigned char)(SUM);
  //image2[y][x] = (unsigned char)(255 - SUM);
#ifdef AXC
  *out = ( (unsigned char)(255 - SUM) ); 
#else
  return( (unsigned char)(255 - SUM) ); 
#endif

}

/* Spatial filtering of image data */
/* Sobel filter (horizontal and veritical differentiation) */
void sobel_filtering()
{
  
  int x, y, i, j;
 
  //The 3x3 window of image
  unsigned char image_window[3][3];
  int rows = 0, cols = 0;

  /* Initialization of image2[y][x] */
  x_size2 = x_size1;
  y_size2 = y_size1;
  for (y = 0; y < y_size2; y++) {
    for (x = 0; x < x_size2; x++) {
      image2[y][x] = 0;
    }
  }

  for (y = 1; y < y_size1 - 1; y++) {
    for (x = 1; x < x_size1 - 1; x++) {
    rows=0; 
    cols = 0;
    for (j = -1; j <= 1; j++) {
	  for (i = -1; i <= 1; i++) {
      image_window[rows][cols] = ( image1[y+j][x+i] );
      cols++;
      }    
      rows++;
      cols = 0;
    }
#ifdef AXC
	unsigned char ret;
//#pragma axc_memoize [(0:5)]{2} 
//[(argument:error bound), ..{out arguments}
//(0:5),(1:5),..]{3}
#pragma axc_memoize [(0:5),(1:5),(2:5),(3:5),(4:5),(6:5),(7:5),(8:5)]{9} 
	kernel_filter(image_window[0][0], image_window[0][1], image_window[0][2],  image_window[1][0],  image_window[1][1],  image_window[1][2],  image_window[2][0],  image_window[2][1],  image_window[2][2], image_window, &ret); 
	image2[y][x] = ret; 
#else
    image2[y][x] = kernel_filter(image_window); 
#endif
    }
  }

}

int main(int argc, char* argv[])
{

  char *pgmInputFileName, *pgmOutputFileName;
  if(argc < 2 || argc > 3) {
      printf("Usage: %s InputFile.pgm <Output.pgm>\n", argv[0]);
      exit(1);
  }
  else if(argc == 2) {
    pgmInputFileName  = argv[1];
    pgmOutputFileName = "default_output.pgm";
  }
  else if(argc == 3) {
    pgmInputFileName  = argv[1];
    pgmOutputFileName = argv[2];
  }
  
  load_image_file( pgmInputFileName  );   /* Input of image1 */ 
  sobel_filtering( );                     /* Sobel filter is applied to image1 */
  save_image_file( pgmOutputFileName );   /* Output of image2 */
  printf("Output saved...OK\n");
  return 0;

}

