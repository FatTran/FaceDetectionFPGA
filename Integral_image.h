#ifndef INTEGRAL_IMAGE_IP_H
#define INTEGRAL_IMAGE_IP_H
#include <ap_int.h>
#include <ap_axi_sdata.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmacro-redefined"

#define IMAGE_WIDTH 1024
#define IMAGE_HEIGHT 1024
#define WINDOW_SIZE 25

typedef int int_I;    // for image window buffer
typedef int int_II;   // for integral image window buffer
typedef int int_SI;   // for square image window buffer
typedef int int_SII;  // for square integral image window buffer

void integralImage(
    ap_uint<8> *input_array,
    ap_uint<32> *output_array,
    ap_uint<32> *sq_output_array,
    int rows,
    int cols
);

#endif
