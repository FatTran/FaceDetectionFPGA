#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmacro-redefined"

#include <ap_int.h>

// Define width for input/output pointers
#define INPUT_PTR_WIDTH 32
#define OUTPUT_PTR_WIDTH 8  
#define CV_IN_TYPE CV_8UC3 
#define CV_OUT_TYPE CV_8UC1 
#define RGB 1  
#define PIXELS_PER_WORD (INPUT_PTR_WIDTH/32)
#define GRAYSCALE_PIXELS_PER_WORD (OUTPUT_PTR_WIDTH/8) 
#define HEIGHT 960
#define WIDTH 720

     

// Function declaration
void rgb2gray(
    ap_uint<INPUT_PTR_WIDTH>* input,
    ap_uint<OUTPUT_PTR_WIDTH>* output,
    int rows,
    int cols
);

#endif
