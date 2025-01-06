#include "rgb2gray.h"
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "common/xf_headers.hpp"


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid Number of Arguments!\nUsage:\n");
        fprintf(stderr, "<Executable Name> <Input image>\n");
        return -1;
    }

    cv::Mat in_src = cv::imread(argv[1], 1);
    if (in_src.data == NULL) {
        fprintf(stderr, "Cannot open image \n");
        return 0;
    }

    cv::Mat in_src_aligned;
    in_src.copyTo(in_src_aligned);

    
    cv::Mat output_gray(in_src.rows, in_src.cols, CV_8UC1);

    rgb2gray(
        (ap_uint<INPUT_PTR_WIDTH> *)in_src_aligned.data,
        (ap_uint<OUTPUT_PTR_WIDTH> *)output_gray.data,
        in_src.rows, in_src.cols);

    cv::imwrite("output_gray.jpg", output_gray);

    return 0;
}