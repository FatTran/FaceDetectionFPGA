#include "rgb2gray.h"
#include <iostream>
#include <opencv2/opencv.hpp>


int main(int argc, char** argv) {
    if(argc != 2) {
        std::cout << "Usage: " << argv[0] << " <input image>" << std::endl;
        return -1;
    }

    cv::Mat input_image = cv::imread(argv[1], 1);

    int rows = input_image.rows;
    int cols = input_image.cols;

    ap_uint<INPUT_PTR_WIDTH>* input_array = new ap_uint<INPUT_PTR_WIDTH>[rows * cols];
    ap_uint<OUTPUT_PTR_WIDTH>* output_array = new ap_uint<OUTPUT_PTR_WIDTH>[rows * cols];

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            cv::Vec3b pixel = input_image.at<cv::Vec3b>(i, j);
            ap_uint<INPUT_PTR_WIDTH> pixel_rgb;
            pixel_rgb(7, 0) = pixel[0];     
            pixel_rgb(15, 8) = pixel[1];    
            pixel_rgb(23, 16) = pixel[2];   
            input_array[i * cols + j] = pixel_rgb;
        }
    }

    rgb2gray(input_array, output_array, rows, cols);

    cv::Mat output_image(rows, cols, CV_8UC1);
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            output_image.at<uchar>(i, j) = output_array[i * cols + j];
        }
    }


    cv::imwrite("output_hls.jpg", output_image);


    return 0;
}
