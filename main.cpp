#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <stdint.h>
#include "haar.h"

const int INITIAL_WIN_WIDTH = 24;
const int MAX_SCALE = 10;


cv::Rect expandRectangle(const cv::Rect& rect, int padding) {
    int x = std::max(0, rect.x - padding);
    int y = std::max(0, rect.y - padding);
    int width = std::min(rect.width + 2 * padding, rect.x + rect.width + padding) - x;
    int height = std::min(rect.height + 2 * padding, rect.y + rect.height + padding) - y;

    return cv::Rect(x, y, width, height);
}
/*void detectAndDrawFace(cv::Mat& colorImage) {
    cv::Mat grayscaleImage;
    cv::cvtColor(colorImage, grayscaleImage, cv::COLOR_BGR2GRAY);

    cv::Mat integralImage;
    cv::integral(grayscaleImage, integralImage);

    bool detectedFace = false;
    cv::Rect faceRect;


    for (int y = 0; y <= grayscaleImage.rows - WIN_WIDTH; ++y) {
        for (int x = 0; x <= grayscaleImage.cols - WIN_WIDTH; ++x) {
            uchar newPixel = grayscaleImage.at<uchar>(y, x);
            int result = detectFace(newPixel, x, y);

            if (result == 1) {
                detectedFace = true;
                faceRect = cv::Rect(x, y, WIN_WIDTH, WIN_WIDTH);
                break;
            }
        }
        if (detectedFace) break;
    }


    if (detectedFace) {
        int padding = 10;
        faceRect = expandRectangle(faceRect, padding);

        cv::rectangle(colorImage, faceRect, cv::Scalar(0, 255, 0), 2);
        std::cout << "Da Phat Hien" << std::endl;
    }
    else {
        std::cout << "Khong Phat Hien" << std::endl;
    }
}

void processImageForFaceDetection(cv::Mat& inputImage) {
    cv::Mat resizedImage;
    cv::Rect detectedFaceRect;


    for (int scale = 1; scale <= MAX_SCALE; ++scale) {
        int newWidth = width * scale;
        int newHeight = height * scale;

        cv::resize(inputImage, resizedImage, cv::Size(newWidth, newHeight));

        detectAndDrawFace(resizedImage);

        if (detectedFaceRect.width > 0 && detectedFaceRect.height > 0) {

            cv::rectangle(resizedImage, detectedFaceRect, cv::Scalar(0, 255, 0), 2);
            std::cout << "Phat hien khuon mat tai kich thuoc: " << newWidth << "x" << newHeight << std::endl;
            break;
        }
    }
}*/


void detectAndDrawFace(cv::Mat& colorImage, int windowWidth) {
    //Chuyển ảnh grayscale và tính integralImage có thể để thành hàm riêng, chuyển xuống hàm scanImageWithVariableWindow để tránh lặp lại.
    cv::Mat grayscaleImage;
    cv::cvtColor(colorImage, grayscaleImage, cv::COLOR_BGR2GRAY);
    cv::Mat integralImage;
    cv::integral(grayscaleImage, integralImage);

    bool detectedFace = false;
    cv::Rect faceRect;

    // Quét qua toàn bộ ảnh
    for (int y = 0; y <= grayscaleImage.rows - windowWidth; ++y) {
        for (int x = 0; x <= grayscaleImage.cols - windowWidth; ++x) {
            uchar newPixel = grayscaleImage.at<uchar>(y, x);
            int result = detectFace(newPixel, x, y);

            if (result == 1) {
                detectedFace = true;
                faceRect = cv::Rect(x, y, windowWidth, windowWidth);
                break;
            }
        }
        if (detectedFace) break;
    }

    if (detectedFace) {
        cv::rectangle(colorImage, faceRect, cv::Scalar(0, 255, 0), 2);
        std::cout << "Da phat hien." << std::endl;
    }
    else {
        std::cout << "Khong phat hien." << std::endl;
    }
}

void scanImageWithVariableWindow(cv::Mat& inputImage) {
    for (int scale = 1; scale <= MAX_SCALE; ++scale) {
        int windowWidth = INITIAL_WIN_WIDTH * scale;
        cv::Mat resizedImage;


        cv::resize(inputImage, resizedImage, cv::Size(inputImage.cols * scale, inputImage.rows * scale));


        detectAndDrawFace(resizedImage, windowWidth);
    }
}


int main() {
    cv::Mat colorImage = cv::imread("C:/Users/ADMIN/Downloads/lfw_funneled/Colin_Prescot/Colin_Prescot_0001.jpg");
    if (colorImage.empty()) {
        std::cerr << "khong the mo anh dau vao" << std::endl;
        return -1;
    }

    scanImageWithVariableWindow(colorImage);

    cv::imshow("Ketqua", colorImage);
    cv::waitKey(0);

    cv::imwrite("C:/MD_Documents/Avad_Topic/IN_OUT/Output/Colin_Prescot_0002.jpg", colorImage);

    return 0;
}
