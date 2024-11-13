#include "haar.h"
void convertToGrayscale(const cv::Mat& rgbImage, cv::Mat& grayscaleImage) {
    cv::cvtColor(rgbImage, grayscaleImage, cv::COLOR_RGB2GRAY, 1);
}

void createIntegralImage(const cv::Mat& input, cv::Mat& integralImage, cv::Mat& SQintegralImage) {
    //CV_Assert(input.type() == CV_16UC1);
    cv::integral(input, integralImage, SQintegralImage, CV_32SC1, CV_32SC1);
}

void preProcessing(cv::Mat& inputImage, cv::Mat& outputImage) {
    cv::Mat grayscaleImage;
    cv::cvtColor(inputImage, grayscaleImage, cv::COLOR_BGR2GRAY);
    cv::integral(grayscaleImage, outputImage);
}

unsigned int int_sqrt(unsigned int value) {
    int i;
    unsigned int a = 0, b = 0, c = 0;
    for (i = 0; i < (32 >> 1); i++)
    {
        c <<= 2;
#define UPPERBITS(value) (value>>30)
        c += UPPERBITS(value);
#undef UPPERBITS
        value <<= 2;
        a <<= 1;
        b = (a << 1) | 1;
        if (c >= b)
        {
            c -= b;
            a++;
        }
    }
    return a;
}

int cascadeClassifier(Point p, int* ii, int* iiSq, int iiWidth, int iiHeight, int iiSqWidth, int iiSqHeight, float factor) {
    int iteratedClassifer = 0;
    int iteratedRect = 0;
    int mean = 0;
    int stddev = 0;
    stddev = iiSq[(p.y) * iiSqWidth + (p.x)];
        - iiSq[(p.y) * iiSqWidth + (p.x + (win_width))] 
        - iiSq[(p.y + (win_height)) * iiSqWidth + (p.x)] 
        + iiSq[(p.y + (win_height)) * iiSqWidth + (p.x + (win_width))];
    
    mean = ii[p.y * iiWidth + p.x] 
        - ii[p.y * iiWidth + (p.x + (win_width))] 
        - ii[(p.y + (win_height)) * iiWidth + p.x] 
        + ii[(p.y + (win_height)) * iiWidth + (p.x + (win_width))];
    
    stddev = stddev * (win_width) * (win_width);
    stddev = stddev - mean * mean;
    
    if (stddev > 0)
    {
        stddev = int_sqrt(stddev);
    }
    else {
        stddev = 1;
    }
    Rect detector = { p.x, p.y, (int) win_width, (int) win_height, 0 };
    float stageTmp = 0.0;
    for (int stageIndex = 0; stageIndex < TOTAL_STAGE; stageIndex++) {
        int classifierStart = iteratedClassifer;
        int classifierEnd = classifierStart + stageOrga[stageIndex];
        stageTmp = 0.0;
        for (int classifierIndex = classifierStart; classifierIndex < classifierEnd; classifierIndex++) {
            //Get node data
            float nodeThresh = stageNodes[classifierIndex * 3];
            float lval = stageNodes[classifierIndex * 3 + 1];
            float rval = stageNodes[classifierIndex * 3 + 2];
            int rectStart = iteratedRect;
            int rectEnd = rectStart + featOrga[classifierIndex];
            int sumA = 0, sumB = 0;
            int sum = 0;
            for (int rectIndex = rectStart; rectIndex < rectEnd; rectIndex++) {
                //Get rectData
                Rect rectData = {
                    stageRects[rectIndex][0] + detector.x,
                    stageRects[rectIndex][1] + detector.y,
                    stageRects[rectIndex][2],
                    stageRects[rectIndex][3],
                    stageRects[rectIndex][4]
                };
                //Get corner of Detector
                int P1 = 0, P2 = 0, P3 = 0, P4 = 0;
                //std::cout << rectData.x << " " << rectData.y << std::endl;
                P1 = (rectData.y - 1) * iiWidth + (rectData.x - 1);
                P2 = (rectData.y - 1) * iiWidth + (rectData.x - 1 + rectData.w);
                P3 = (rectData.y - 1 + rectData.h) * iiWidth + (rectData.x - 1);
                P4 = (rectData.y - 1 + rectData.h) * iiWidth + (rectData.x - 1 + rectData.w);
                //std::cout << P1 << " " << P2 << " " << P3 << " " << P4 << std::endl;
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
                /*sumA = ii[P4] - ii[P3];
                sumB = ii[P1] - ii[P2];*/
                sum += ((sumA + sumB)) * rectData.weight;
            }
            
            iteratedRect += featOrga[classifierIndex];
            if (sum < nodeThresh * stddev) {
                stageTmp += lval;
            }
            else {
                stageTmp += rval;
            }
        }
        iteratedClassifer += stageOrga[stageIndex];
        //std::cout << "Calculate stage sum: " << stageTmp << std::endl;
        //std::cout << "Stage threshold: " << stageThresholds[stageIndex] << std::endl;
        //printf("%f\t", stageTmp);
        if (stageTmp < stageThresholds[stageIndex]) {
            return -stageIndex;
        }
    }
    return 1;
}
void Mat2Array(cv::Mat inputMat, int* & outputArray) {
    int MatHeight = inputMat.rows;
    int MatWidth = inputMat.cols;
    for (int y = 1; y < MatHeight; y++) {
        int* inputMaty = inputMat.ptr<int>(y);
        for (int x = 1; x < MatWidth; x++) {
            outputArray[(y - 1) * (MatWidth - 1) + (x - 1)] = inputMaty[x];
        }
    }
}
void detectFace(cv::Mat& inputImage, float scaleFactor) {
    cv::Mat grayScaleImage;
    cv::Mat integralImage;
    cv::Mat SQintegralImage;
    cv::Mat resizedImage;
    int imageHeight = inputImage.rows;
    int imageWidth = inputImage.cols;
    float factor = 1; 
    while (imageHeight / factor > WIN_HEIGHT && imageWidth / factor > WIN_WIDTH) {
        /*win_width = WIN_WIDTH * factor;
        win_height = WIN_HEIGHT * factor;*/
        cv::resize(inputImage, resizedImage, cv::Size(), 1 / factor, 1 / factor, cv::INTER_AREA);
        convertToGrayscale(resizedImage, grayScaleImage);
        createIntegralImage(grayScaleImage, integralImage, SQintegralImage);

        int iiHeight = integralImage.rows - 1;
        int iiWidth = integralImage.cols - 1;
        int iiSqHeight = SQintegralImage.rows - 1;
        int iiSqWidth = SQintegralImage.cols - 1;

        int* ii = new int[iiHeight * iiWidth];
        int* iiSq = new int[iiSqHeight * iiSqWidth];
        Mat2Array(integralImage, ii);
        Mat2Array(SQintegralImage, iiSq);

        Point p;
        int rows = resizedImage.rows;
        int cols = resizedImage.cols;

        for (int y = 0; y < rows - win_height; y++) {
            for (int x = 0; x < cols - win_width; x++) {
                p = { x , y };
                int result = cascadeClassifier(p, ii, iiSq, iiWidth, iiHeight, iiSqWidth, iiSqHeight, factor);
                if (result > 0) {
                    cv::Rect rect(p.x * factor, p.y * factor, win_width * factor, win_height * factor);
                    cv::rectangle(inputImage, rect, cv::Scalar(0, 255, 0));
                }
            }
        }
        factor = factor * scaleFactor;
        delete[] ii;
        ii = nullptr;
        delete[] iiSq;
        iiSq = nullptr;
    }
    return;
}
