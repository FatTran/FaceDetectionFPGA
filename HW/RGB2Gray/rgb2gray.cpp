#include "rgb2gray.h"
#include <common/xf_common.hpp>


void rgb2gray(
    ap_uint<INPUT_PTR_WIDTH>* input,
    ap_uint<OUTPUT_PTR_WIDTH>* output,
    int rows,
    int cols
) {
    #pragma HLS INTERFACE m_axi port=input offset=slave bundle=gmem0 depth = (HEIGHT * WIDTH * 3) / (INPUT_PTR_WIDTH/8)
    #pragma HLS INTERFACE m_axi port=output offset=slave bundle=gmem1 depth = (HEIGHT * WIDTH) / (OUTPUT_PTR_WIDTH/8)
    #pragma HLS INTERFACE s_axilite port=rows
    #pragma HLS INTERFACE s_axilite port=cols
    #pragma HLS INTERFACE s_axilite port=return

    xf::cv::Mat<XF_8UC3, HEIGHT, WIDTH, XF_NPPC1> src(rows, cols);
    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> dst(rows, cols);

#pragma HLS stream variable=src.data depth=_XFCVDEPTH_DEFAULT
#pragma HLS stream variable=dst.data depth=_XFCVDEPTH_DEFAULT

#pragma HLS DATAFLOW

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            int offset = i * cols + j;
            ap_uint<24> pixel = input[offset];
            src.write(offset, pixel);
        }
    }

//    Array2xfMat<INPUT_PTR_WIDTH, XF_8UC3, HEIGHT, WIDTH, XF_NPPC1>(input, src);

    xf::cv::bgr2gray<XF_8UC3, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(src, dst);

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            int offset = i * cols + j;
            output[offset] = dst.read(offset);
        }
    }
//    xfMat2Array<OUTPUT_PTR_WIDTH, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(dst, output);
}
