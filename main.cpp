#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "haar.h"


//C:/Users/ADMIN/Downloads/lfw_funneled/Colin_Prescot/Colin_Prescot_0001.jpg
int main() {
    cv::Mat inputImage = cv::imread("C:/Users/ADMIN/Downloads/lfw_funneled/Dino_de_Laurentis/Dino_de_Laurentis_0002.jpg"); // Load your test image here

    if (inputImage.empty()) {
        std::cerr << "Failed to load image." << std::endl;
        return -1;
    }

   
    processImage(inputImage);

    cv::imshow("Face Detection", inputImage);
    cv::waitKey(0);
    return 0;
}
