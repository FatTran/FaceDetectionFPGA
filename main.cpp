#include "haar.h"
#include <chrono>

int main() {
    std::ios::sync_with_stdio(false);
    cv::Mat colorImage = cv::imread("C:/Users/Phat/Downloads/29_(1).jpg");
    if (colorImage.empty()) {
        std::cerr << "khong the mo anh dau vao" << std::endl;
        return -1;
    }
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "Can't open device" << std::endl;
    }
    bool done = false;
    cv::Mat cameraImage;
    cv::Mat scaledImage;
    /*while (!done) {
        cap >> cameraImage;
        if (cameraImage.empty()) {
            break;
        }
        cv::resize(cameraImage, scaledImage, cv::Size(), 0.5, 0.5, cv::INTER_AREA);
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        detectFace(scaledImage, 1.2);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
        cv::imshow("Output image", scaledImage);
        int key = cv::waitKey(1);
        if (key == 'x') {
            break;
        }
    }*/
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    detectFace(colorImage, 1.2);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
    cv::imshow("Output image", colorImage);
    cv::waitKey(0);
    return 0;
}
