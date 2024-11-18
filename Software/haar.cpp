#include "haar.h"
void convertToGrayscale(const cv::Mat& rgbImage, cv::Mat& grayscaleImage) {
    cv::cvtColor(rgbImage, grayscaleImage, cv::COLOR_RGB2GRAY);
}

void createIntegralImage(const cv::Mat& input, cv::Mat& integralImage) {
    CV_Assert(input.type() == CV_8UC1);
    cv::integral(input, integralImage, CV_32FC1);
}

void preProcessing(cv::Mat& inputImage, cv::Mat& outputImage) {
    cv::Mat grayscaleImage;
    cv::cvtColor(inputImage, grayscaleImage, cv::COLOR_BGR2GRAY);
    cv::integral(grayscaleImage, outputImage);
}

void evaluateRect(cv::Mat integralImage, int rectIndex, int& nodeSum) {
    //Calculate the rectangle value based on the feature input
    //Get Rect data
    /*int8_t xRect = stageRects[rectIndex][0];
    int8_t yRect = stageRects[rectIndex][1];
    int8_t wRect = stageRects[rectIndex][2];
    int8_t hRect = stageRects[rectIndex][3];
    int8_t weight = stageRects[rectIndex][4];*/
    //Get IntegralImage corners

}

void evaluateNode(cv::Mat integralImage, int nodeIndex,int& stageSum) {
    //Evaluate each node based on feature input
}

void evaluateStage(Point p, cv::Mat inegralImage, int stageindex) {
    //Evaluate each stage
}

bool cascadeClassifier(Point p, int* ii, int iiWidth, int iiHeight, float factor) {
    int iteratedClassifer = 0;
    int iteratedRect = 0;
    Rect detector = { p.x, p.y, (int) WIN_WIDTH * factor, (int) WIN_HEIGHT * factor, 0 };
    float stageTmp = 0.0;
    for (int stageIndex = 0; stageIndex < TOTAL_STAGE; stageIndex++) {
        int classifierStart = iteratedClassifer;
        int classifierEnd = classifierStart + stageOrga[stageIndex];
        for (int classifierIndex = classifierStart; classifierIndex < classifierEnd; classifierIndex++) {
            //Get node data
            float nodeThresh = stageNodes[classifierIndex * 3];
            float lval = stageNodes[classifierIndex * 3 + 1];
            float rval = stageNodes[classifierIndex * 3 + 2];
            int rectStart = iteratedRect;
            int rectEnd = rectStart + featOrga[classifierIndex];
            int sumA = 0, sumB = 0, sum = 0;
            for (int rectIndex = rectStart; rectIndex < rectEnd; rectIndex++) {
                //Get rectData
                Rect rectData = {
                    stageRects[rectIndex][0] + detector.x,
                    stageRects[rectIndex][1] + detector.y,
                    stageRects[rectIndex][2] * factor,
                    stageRects[rectIndex][3] * factor,
                    stageRects[rectIndex][4]
                };
                //Get corner of Detector
                int P1 = 0, P2 = 0, P3 = 0, P4 = 0;
                
                P1 = (rectData.y - 1) * iiWidth + (rectData.x - 1);
                P2 = (rectData.y - 1) * iiWidth + (rectData.x + rectData.w);
                P3 = (rectData.y + rectData.h) * iiWidth + (rectData.x);
                P4 = (rectData.y + rectData.h) * iiWidth + (rectData.x + rectData.w);
                if (rectData.x == 0) {
                    if (rectData.y == 0) {
                        sumA = ii[P4];
                        sumB = 0;
                    }
                    else {
                        sumA = ii[P4];
                        sumB = 0 - ii[P2];
                    }
                }
                else {
                    if (rectData.y == 0) {
                        sumA = ii[P4] - ii[P3];
                        sumB = 0;
                    }
                    else {
                        sumA = ii[P4] - ii[P3];
                        sumB = ii[P1] - ii[P2];
                    }
                }
                sum += ((sumA + sumB)) * rectData.weight;
            }
            iteratedRect += featOrga[classifierIndex];
            if (sum < nodeThresh) {
                stageTmp += lval;
            }
            else {
                stageTmp += rval;
            }
        }
        iteratedClassifer += stageOrga[stageIndex];
        //std::cout << "Calculate stage sum: " << stageTmp << std::endl;
        //std::cout << "Stage threshold: " << stageThresholds[stageIndex] << std::endl;
        if (stageTmp < stageThresholds[stageIndex]) {
            return false;
        }
    }
    return true;
}

void detectFace(cv::Mat& inputImage, float scaleFactor) {
    //1. Convert Matrix to 1D array
    //2. Iterate through the integralImage with a detector of 24x24 in size
    //3. Evaluate all stages to find box with face
    //4. DownScale input image (Use nearest neighbor)
    //5. Repeat 2 until the image is smaller than the detector window
    cv::Mat grayScaleImage;
    cv::Mat integralImage;
    convertToGrayscale(inputImage, grayScaleImage);
    createIntegralImage(grayScaleImage, integralImage);
    //cv::imshow("Gray image", grayScaleImage);
    int iiHeight = integralImage.rows;
    int iiWidth = integralImage.cols;
    int* ii = new int[iiHeight * iiWidth];
    //std::vector<int> ii;
    for (int y = 0; y < iiHeight; y++) {
        int* IIy = integralImage.ptr<int>(y);
        for (int x = 0; x < iiWidth; x++) {
            ii[y * iiWidth + x] = IIy[x];
            //ii.push_back(IIy[x]);
        }
    }
    Point p;
    float factor = 1;
    while (iiHeight > WIN_HEIGHT * factor && iiWidth > WIN_WIDTH * factor) {
        //std::cout << factor << std::endl;
        for (int y = 0; y < iiHeight - WIN_HEIGHT * factor; y++) {
            for (int x = 0; x < iiWidth - WIN_WIDTH * factor; x++) {
                p = { x , y };
                if (cascadeClassifier(p, ii, iiWidth, iiHeight, factor)) {
                    cv::Rect rect(p.x, p.y, (int) WIN_WIDTH * factor, (int) WIN_HEIGHT * factor);
                    cv::rectangle(inputImage, rect, cv::Scalar(0, 255, 0));
                }
            }
        }
        factor = factor * scaleFactor;
    }
    delete ii;
    ii = nullptr;
    return;
}
