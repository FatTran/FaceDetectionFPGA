#include "rgb2gray.h"
#include <common/xf_common.hpp>


void rgb2gray(
    ap_uint<INPUT_PTR_WIDTH>* input,
    ap_uint<OUTPUT_PTR_WIDTH>* output,
    int rows,
    int cols
) {
    #pragma HLS INTERFACE m_axi port=input offset=slave bundle=gmem0 depth = (ROWS * COLS * 3) / (INPUT_PTR_WIDTH/8)
    #pragma HLS INTERFACE m_axi port=output offset=slave bundle=gmem1 depth = (ROWS * COLS) / (OUTPUT_PTR_WIDTH/8)
    #pragma HLS INTERFACE s_axilite port=rows bundle=CONTROL_BUS
    #pragma HLS INTERFACE s_axilite port=cols bundle=CONTROL_BUS
    #pragma HLS INTERFACE s_axilite port=return bundle=CONTROL_BUS

    xf::cv::Mat<XF_8UC3, HEIGHT, WIDTH, XF_NPPC1> src(rows, cols);
    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> dst(rows, cols);

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            int offset = i * cols + j;
            ap_uint<24> pixel = input[offset];
            src.write(offset, pixel);
        }
    }

    xf::cv::bgr2gray<XF_8UC3, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(src, dst);

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            int offset = i * cols + j;
            output[offset] = dst.read(offset);
        }
    }
}
