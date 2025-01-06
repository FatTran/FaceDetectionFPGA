#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "haarcascade_frontalface_default.h"

void preProcessing(cv::Mat& inputImg, cv::Mat& outPut) {
    cv::Mat grayImg;
    cv::cvtColor(inputImg, grayImg, cv::COLOR_BGR2GRAY);
    cv::integral(grayImg, outPut);
    cv::imshow("Gray img", grayImg);
}

//void detectFace();

int main() {
    cv::Mat inputImg = cv::imread("D:/UTE/Advanced_Topic/SW/img/1.jpg");
    cv::Mat integralImg;
    preProcessing(inputImg, integralImg);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
