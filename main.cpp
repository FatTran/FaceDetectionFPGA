#include "haar.h"
#include <chrono>

int main() {
    std::ios::sync_with_stdio(false);
    cv::Mat colorImage = cv::imread("C:/Users/Phat/Downloads/29_(1).jpg");
    if (colorImage.empty()) {
        std::cerr << "khong the mo anh dau vao" << std::endl;
        return -1;
    }
    cv::Mat grayScaleImage;
    cv::Mat integralImage;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    detectFace(colorImage, 1.25);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
    cv::imshow("Output image", colorImage);
    //scanImageWithVariableWindow(colorImage);
    //convertToGrayscale(colorImage, grayScaleImage);
    //createIntegralImage(grayScaleImage, integralImage);
    //cascadeClassifier()
    //detectFace(integralImage, 0, 0, 0);
    //cv::imshow("Ketqua", colorImage);
    cv::waitKey(0);
    //detectFace(integralImage, 0);
    //cv::imwrite("D:/UTE/Advanced_Topic/SW/Output/img.jpg", colorImage);
    return 0;
}
