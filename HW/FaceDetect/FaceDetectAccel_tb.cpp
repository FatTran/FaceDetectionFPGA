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
    ap_uint<32> detectedX[100];
    ap_uint<32> detectedY[100];
    ap_uint<32> detectedWidth[100];
    ap_uint<32> detectedHeight[100];

    int ii[HEIGHT * WIDTH];
    int iiSq[HEIGHT * WIDTH];

    while((int) HEIGHT / scaleFactor > WIN_HEIGHT && (int) WIDTH / scaleFactor > WIN_WIDTH){
        cv::resize(inputImage, resizedImage, cv::Size(), 1 / scaleFactor, 1 / scaleFactor, cv::INTER_LINEAR);
        convertToGrayscale(resizedImage, grayScaleImage);
        createIntegralImage(grayScaleImage, integralImage, SQintegralImage);
        int iiHeight = integralImage.rows, iiWidth = integralImage.cols;
        int iiSqHeight = SQintegralImage.rows, iiSqWidth = SQintegralImage.cols;
        Mat2Array(integralImage, ii);
        Mat2Array(SQintegralImage, iiSq);

        FaceDetect(			(ap_uint<32>*)ii,
							(ap_uint<32>*)iiSq,
							iiHeight,
							iiWidth,
							(ap_int<32>*)stageOrga,
							(ap_int<32>*)stageThresholds,
							(ap_int<32>*)stageNodes,
							(ap_int<32>*)featOrga,
							(ap_int<32>*)stageRects,
							scaleFactor,
							detectedX,
							detectedY,
							detectedWidth,
							detectedHeight
        );
        for(int i = 0 ; i < 100 ; i++){
			cv::Rect rect(detectedX[i], detectedY[i], detectedWidth[i], detectedHeight[i]);
			cv::rectangle(inputImage, rect, cv::Scalar(0, 255, 0), 1);
        }
        scaleFactor = scaleFactor * 1.2;
    }
    cv::imwrite("output.png", inputImage);
    return 0;
}
