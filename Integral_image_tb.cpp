#include "Integral_image.h"
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

    cv::Mat rgb2grey;
    rgb2grey = cv::imread(argv[1], 0);  // Đọc ảnh grayscale trực tiếp

    if (rgb2grey.data == NULL) {
        fprintf(stderr, "Cannot open image \n");
        return 0;
    }

    /*
    if (rgb2grey.rows > IMAGE_HEIGHT || rgb2grey.cols > IMAGE_WIDTH) {
        fprintf(stderr, "Image size exceeds maximum allowed dimensions\n");
        return -1;
    }
    */
    cv::Mat rgb2grey_align;
    rgb2grey.copyTo(rgb2grey_align);

    std::vector<ap_uint<32>> output_integral(rgb2grey.rows * rgb2grey.cols);
    std::vector<ap_uint<32>> output_sq_integral(rgb2grey.rows * rgb2grey.cols);

    
    std::vector<ap_uint<8>> input_gray(rgb2grey.rows * rgb2grey.cols);
    for (int i = 0; i < rgb2grey.rows; i++) {
        for (int j = 0; j < rgb2grey.cols; j++) {
            input_gray[i * rgb2grey.cols + j] = rgb2grey.at<uchar>(i, j);
        }
    }

    
    integralImage(
        (ap_uint<8>*)input_gray.data(), 
        (ap_uint<32>*)output_integral.data(), 
        (ap_uint<32>*)output_sq_integral.data(), 
        rgb2grey.rows, 
        rgb2grey.cols
    );

    // Tạo ảnh để hiển thị kết quả
    cv::Mat output_integral_img(rgb2grey.rows, rgb2grey.cols, CV_8UC1);
    cv::Mat output_sq_integral_img(rgb2grey.rows, rgb2grey.cols, CV_8UC1);

    // Chuyển đổi kết quả sang ảnh
    for (int i = 0; i < rgb2grey.rows; i++) {
        for (int j = 0; j < rgb2grey.cols; j++) {
            int idx = i * rgb2grey.cols + j;
            output_integral_img.at<uchar>(i, j) = cv::saturate_cast<uchar>(output_integral[idx] / 255);
            output_sq_integral_img.at<uchar>(i, j) = cv::saturate_cast<uchar>(output_sq_integral[idx] / 255);
        }
    }

    // Lưu kết quả
    cv::imwrite("output_integral.jpg", output_integral_img);
    cv::imwrite("output_sq_integral.jpg", output_sq_integral_img);

    return 0;
}

