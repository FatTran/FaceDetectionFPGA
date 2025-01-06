#ifndef RGB2GRAY_H
#define RGB2GRAY_H

#include <ap_int.h>
#include "hls_stream.h"
#include "common/xf_common.hpp"
#include "common/xf_utility.hpp"
#include "imgproc/xf_cvt_color.hpp"
//#include "E:/Vitis_HLS/2024.1/Vitis_Libraries/vision/L1/include/imgproc/xf_cvt_color.hpp"

#ifndef INPUT_PTR_WIDTH
#define INPUT_PTR_WIDTH 24
#endif

#ifndef OUTPUT_PTR_WIDTH
#define OUTPUT_PTR_WIDTH 8
#endif

#define HEIGHT 960
#define WIDTH 720

#define TYPE XF_8UC3
#define TYPE_OUT XF_8UC1
#define NPC1 XF_NPPC1

// #define ROWS 720
// #define COLS 1080

void rgb2gray(
    ap_uint<INPUT_PTR_WIDTH>* input,
    ap_uint<OUTPUT_PTR_WIDTH>* output,
    int rows,
    int cols
);

#endif 
