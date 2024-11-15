#include "haar.h"
void convertToGrayscale(const cv::Mat& rgbImage, cv::Mat& grayscaleImage) {
    cv::cvtColor(rgbImage, grayscaleImage, cv::COLOR_RGB2GRAY, 1);
}

void createIntegralImage(const cv::Mat& input, cv::Mat& integralImage, cv::Mat& SQintegralImage) {
    //CV_Assert(input.type() == CV_16UC1);
    cv::integral(input, integralImage, SQintegralImage, CV_8UC1, CV_8UC1);
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
    -iiSq[(p.y) * iiSqWidth + (p.x + (win_width))]
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
    Rect detector = { p.x, p.y, (int)win_width, (int)win_height, 0 };
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
                
                Rect rectData = {
                    stageRects[rectIndex][0] + detector.x,
                    stageRects[rectIndex][1] + detector.y,
                    stageRects[rectIndex][2],
                    stageRects[rectIndex][3],
                    stageRects[rectIndex][4]
                };
                
                int P1 = 0, P2 = 0, P3 = 0, P4 = 0;
                
                P1 = (rectData.y - 1) * iiWidth + (rectData.x - 1);
                P2 = (rectData.y - 1) * iiWidth + (rectData.x - 1 + rectData.w);
                P3 = (rectData.y - 1 + rectData.h) * iiWidth + (rectData.x - 1);
                P4 = (rectData.y - 1 + rectData.h) * iiWidth + (rectData.x - 1 + rectData.w);
               
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
void Mat2Array(cv::Mat inputMat, int*& outputArray) {
    int MatHeight = inputMat.rows;
    int MatWidth = inputMat.cols;
    for (int y = 1; y < MatHeight; y++) {
        int* inputMaty = inputMat.ptr<int>(y);
        for (int x = 1; x < MatWidth; x++) {
            outputArray[(y - 1) * (MatWidth - 1) + (x - 1)] = inputMaty[x];
        }
    }
}

///
void groupBoundingBoxes(std::vector<cv::Rect>& rectList, int groupThreshold, double eps) {
    if (groupThreshold <= 0 || rectList.empty()) {
        return; 
    }

    
    std::vector<int> labels;
    int nclasses = cv::partition(rectList, labels, [eps](const cv::Rect& r1, const cv::Rect& r2) {
        double deltaX = std::abs(r1.x - r2.x);
        double deltaY = std::abs(r1.y - r2.y);
        double deltaW = std::abs(r1.width - r2.width);
        double deltaH = std::abs(r1.height - r2.height);

        return deltaX <= r1.width * eps && deltaY <= r1.height * eps &&
            deltaW <= r1.width * eps && deltaH <= r1.height * eps;
        });

    
    std::vector<cv::Rect> groupedRects(nclasses);
    std::vector<int> groupWeights(nclasses, 0);

    for (size_t i = 0; i < rectList.size(); i++) {
        int cls = labels[i];
        groupedRects[cls].x += rectList[i].x;
        groupedRects[cls].y += rectList[i].y;
        groupedRects[cls].width += rectList[i].width;
        groupedRects[cls].height += rectList[i].height;        
        groupWeights[cls]++;
    }

    
    rectList.clear();
    for (int i = 0; i < nclasses; i++) {
        if (groupWeights[i] > groupThreshold) {
            groupedRects[i].x /= groupWeights[i];
            groupedRects[i].y /= groupWeights[i];
            groupedRects[i].width /= groupWeights[i];
            groupedRects[i].height /= groupWeights[i];
            rectList.push_back(groupedRects[i]);
        }
    }
}
void detectFace(cv::Mat& inputImage, float scaleFactor) {
    //cv::resize(inputImage, inputImage, cv::Size(720, 480));
    cv::resize(inputImage, inputImage, cv::Size(240, 480));
    cv::Mat grayScaleImage, integralImage, SQintegralImage, resizedImage;
    int imageHeight = inputImage.rows, imageWidth = inputImage.cols;
    float factor = 1;
    std::vector<cv::Rect> detectedRects;

    while (imageHeight / factor > WIN_HEIGHT && imageWidth / factor > WIN_WIDTH) {
        cv::resize(inputImage, resizedImage, cv::Size(), 1 / factor, 1 / factor, cv::INTER_AREA);
        convertToGrayscale(resizedImage, grayScaleImage);
        createIntegralImage(grayScaleImage, integralImage, SQintegralImage);

        int iiHeight = integralImage.rows - 1, iiWidth = integralImage.cols - 1;
        int iiSqHeight = SQintegralImage.rows - 1, iiSqWidth = SQintegralImage.cols - 1;

        int* ii = new int[iiHeight * iiWidth];
        int* iiSq = new int[iiSqHeight * iiSqWidth];
        Mat2Array(integralImage, ii);
        Mat2Array(SQintegralImage, iiSq);

        Point p;
        int rows = resizedImage.rows, cols = resizedImage.cols;

        for (int y = 0; y < rows - win_height; y++) {
            for (int x = 0; x < cols - win_width; x++) {
                p = { x, y };
                int result = cascadeClassifier(p, ii, iiSq, iiWidth, iiHeight, iiSqWidth, iiSqHeight, factor);
                if (result > 0) {
                    cv::Rect rect(p.x * factor, p.y * factor, win_width * factor, win_height * factor);
                    detectedRects.push_back(rect);
                }
            }
        }

        factor *= scaleFactor;
        delete[] ii;
        delete[] iiSq;
    }

    
    groupBoundingBoxes(detectedRects, 2, 0.2); 

    
    for (const auto& rect : detectedRects) {
        cv::rectangle(inputImage, rect, cv::Scalar(0, 255, 0), 2);
    }
}

//void detectFace(cv::Mat& inputImage, float scaleFactor) {
//    cv::resize(inputImage, inputImage, cv::Size(720, 480));
//    //cv::resize(inputImage, inputImage, cv::Size(480, 720));
//    cv::Mat grayScaleImage;
//    cv::Mat integralImage;
//    cv::Mat SQintegralImage;
//    cv::Mat resizedImage;
//  
//    int imageHeight = inputImage.rows;
//    int imageWidth = inputImage.cols;
//    float factor = 1;
//;
//
//    while (imageHeight / factor > WIN_HEIGHT && imageWidth / factor > WIN_WIDTH) {
//        /*win_width = WIN_WIDTH * factor;
//        win_height = WIN_HEIGHT * factor;*/
//        cv::resize(inputImage, resizedImage, cv::Size(), 1 / factor, 1 / factor, cv::INTER_AREA);
//        convertToGrayscale(resizedImage, grayScaleImage);
//        createIntegralImage(grayScaleImage, integralImage, SQintegralImage);
//
//        int iiHeight = integralImage.rows - 1;
//        int iiWidth = integralImage.cols - 1;
//        int iiSqHeight = SQintegralImage.rows - 1;
//        int iiSqWidth = SQintegralImage.cols - 1;
//
//        int* ii = new int[iiHeight * iiWidth];
//        int* iiSq = new int[iiSqHeight * iiSqWidth];
//        Mat2Array(integralImage, ii);
//        Mat2Array(SQintegralImage, iiSq);
//
//        Point p;
//        int rows = resizedImage.rows;
//        int cols = resizedImage.cols;
//
//        for (int y = 0; y < rows - win_height; y++) {
//            for (int x = 0; x < cols - win_width; x++) {
//                p = { x , y };
//                int result = cascadeClassifier(p, ii, iiSq, iiWidth, iiHeight, iiSqWidth, iiSqHeight, factor);
//                if (result > 0) {
//                    cv::Rect rect(p.x * factor, p.y * factor, win_width * factor, win_height * factor);
//                    cv::rectangle(inputImage, rect, cv::Scalar(0, 255, 0));
//                }
//                
//            }
//        }
//        factor = factor * scaleFactor;
//        delete[] ii;
//        ii = nullptr;
//        delete[] iiSq;
//        iiSq = nullptr;
//    }
//    return;
//}

