#include "xf_config_params.h"
#include "haar_data.h"
//#include "FaceDetectAccel.cpp"
void convertToGrayscale(const cv::Mat& rgbImage, cv::Mat& grayscaleImage) {
    cv::cvtColor(rgbImage, grayscaleImage, cv::COLOR_RGB2GRAY, 1);
}

void createIntegralImage(const cv::Mat& input, cv::Mat& integralImage, cv::Mat& SQintegralImage) {
    //CV_Assert(input.type() == CV_16UC1);
    cv::integral(input, integralImage, SQintegralImage, CV_32SC1, CV_32SC1);
}
void Mat2Array(cv::Mat inputMat, int* outputArray) {
    int MatHeight = inputMat.rows;
    int MatWidth = inputMat.cols;
    for (int y = 0; y < MatHeight; y++) {
        int* inputMaty = inputMat.ptr<int>(y);
        for (int x = 0; x < MatWidth; x++) {
            outputArray[(y) * (MatWidth) + (x)] = inputMaty[x];
        }
    }
}

int main(){
    cv::Mat inputImage = cv::imread("C:/Users/Phat/Downloads/29_(1).jpg");
    cv::Mat grayScaleImage;
    cv::Mat resizedImage;
    cv::Mat integralImage;
    cv::Mat SQintegralImage;
    float scaleFactor = 1;
    ap_uint<32> detectedX = 0;
    ap_uint<32> detectedY = 0;
    ap_uint<32> detectedWidth = 0;
    ap_uint<32> detectedHeight = 0;

    int ii[HEIGHT * WIDTH];
    int iiSq[HEIGHT * WIDTH];

    //int stageOrga[1] = {9};
    //int stageThresholds[1] = {-5042};
    //int stageNodes[27] = {-31, 2087, -2217, 12, -1863, 1327, 21, -1510, 1062, 5, -874, 1176, 15, -779, 1260, 99, 557, -1874, 2, -1691, 440, -18, -1476, 443, 5, -859, 852};
    //int featOrga[9] = {2, 2, 2, 2, 2, 2, 2, 2, 2};
    //int stageRects[90] = {6, 4, 12, 9, -1, 6, 7, 12, 3, 3, 6, 4, 12, 7, -1, 10, 4, 4, 7, 3, 3, 9, 18, 9, -1, 3, 12, 18, 3, 3, 8, 18, 9, 6, -1, 8, 20, 9, 2, 3, 3, 5, 4, 19, -1, 5, 5, 2, 19, 2, 6, 5, 12, 16, -1, 6, 13, 12, 8, 2, 5, 8, 12, 6, -1, 5, 11, 12, 3, 2, 11, 14, 4, 10, -1, 11, 19, 4, 5, 2, 4, 0, 7, 6, -1, 4, 3, 7, 3, 2};
    while((int) HEIGHT / scaleFactor > WIN_HEIGHT && (int) WIDTH / scaleFactor > WIN_WIDTH){
        cv::resize(inputImage, resizedImage, cv::Size(), 1 / scaleFactor, 1 / scaleFactor, cv::INTER_AREA);
        convertToGrayscale(resizedImage, grayScaleImage);
        createIntegralImage(grayScaleImage, integralImage, SQintegralImage);
        int iiHeight = integralImage.rows, iiWidth = integralImage.cols;
        int iiSqHeight = SQintegralImage.rows, iiSqWidth = SQintegralImage.cols;
        Mat2Array(integralImage, ii);
        Mat2Array(SQintegralImage, iiSq);

        FaceDetectAccel(	(ap_int<32>*)ii,
							(ap_int<32>*)iiSq,
							iiHeight,
							iiWidth,
							(ap_int<32>*)stageOrga,
							(ap_int<32>*)stageThresholds,
							(ap_int<32>*)stageNodes,
							(ap_int<32>*)featOrga,
							(ap_int<32>*)stageRects,
							scaleFactor,
							&detectedX,
							&detectedY,
							&detectedWidth,
							&detectedHeight
        );
        cv::Rect rect(detectedX, detectedY, detectedWidth, detectedHeight);
        cv::rectangle(inputImage, rect, cv::Scalar(0, 255, 0), 1);
        scaleFactor = scaleFactor * 1.2;
    }
    cv::imwrite("output.png", inputImage);
    return 0;
}
