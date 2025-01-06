#include "haarcascade.h"
void convertToGrayscale(const cv::Mat& rgbImage, cv::Mat& grayscaleImage) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    cv::cvtColor(rgbImage, grayscaleImage, cv::COLOR_RGB2GRAY, 1);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
}

void convertToGrayscaleCustom(const cv::Mat& rgbImage, cv::Mat& grayscaleImage) {
    std::vector<cv::Mat> channels(3);
    cv::split(rgbImage, channels);
    cv::Mat R;
    cv::Mat G;
    cv::Mat B;
    B = channels[2];
    G = channels[1];
    R = channels[0];

    for (int i = 0; i < rgbImage.rows; i++) {
        for (int j = 0; j < rgbImage.cols; j++) {


            uint8_t r = R.at<uint8_t>(i, j);
            uint8_t g = G.at<uint8_t>(i, j);
            uint8_t b = B.at<uint8_t>(i, j);

            //0.299f*r + 0.587f*g + 0.114f*b 
            uint8_t gray = (r * 77 + g * 150 + b * 29) >> 8;

            grayscaleImage.at<uint8_t>(i, j) = gray;
        }
    }
}

void createIntegralImage(const cv::Mat& input, cv::Mat& integralImage, cv::Mat& SQintegralImage) {
    //CV_Assert(input.type() == CV_16UC1);
    //cv::Mat SQimg;
    //cv::multiply(input, input, SQimg);
    cv::integral(input, integralImage, SQintegralImage, CV_32SC1, CV_32SC1);
    /*cv::integral(input, integralImage, CV_32SC1);
    cv::integral(SQimg, SQintegralImage, CV_32SC1);*/
    //std::cout << SQintegralImage.at<uint32_t>(3, 1) << std::endl;;

}

void preProcessing(cv::Mat& inputImage, cv::Mat& outputImage) {
    cv::Mat grayscaleImage;
    cv::cvtColor(inputImage, grayscaleImage, cv::COLOR_BGR2GRAY);
    cv::integral(grayscaleImage, outputImage);
}
//
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
    /*int iteratedClassifer = 0;
    int iteratedRect = 0;*/
    int classifierStart = 0;
    int classifierEnd = 0;
    int rectStart = 0;
    int rectEnd = 0;
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

    stddev = (stddev > 0) ? int_sqrt(stddev) : 1;
    Rect detector = { p.x, p.y, (int)win_width, (int)win_height, 0 };
    //int stageTmp = 0;
    //StageEvalAccel:
    for (int stageIndex = 0; stageIndex < TOTAL_STAGE; stageIndex++) {
        //int classifierStart = iteratedClassifer;
        classifierStart = classifierEnd;
        classifierEnd = classifierStart + stageOrga[stageIndex];
        int stageTmp = 0;
        //NodeEvalAccel:
        for (int classifierIndex = classifierStart; classifierIndex < classifierEnd; classifierIndex++) {
            //Get node data
            int nodeThresh = stageNodes[classifierIndex * 3];
            int lval = stageNodes[classifierIndex * 3 + 1];
            int rval = stageNodes[classifierIndex * 3 + 2];
            /*int rectStart = iteratedRect;*/
            rectStart = rectEnd;
            rectEnd = rectStart + featOrga[classifierIndex];
            int sumA = 0, sumB = 0;
            int sum = 0;
            //RectEvalAccel (detectorData, rectStart, rectEnd, iidata):
            for (int rectIndex = rectStart; rectIndex < rectEnd; rectIndex++) {
                Rect rectData = {
                    stageRects[rectIndex*5 + 0] + detector.x,
                    stageRects[rectIndex*5 + 1] + detector.y,
                    stageRects[rectIndex*5 + 2],
                    stageRects[rectIndex*5 + 3],
                    stageRects[rectIndex*5 + 4]
                };
                
                int P1 = 0, P2 = 0, P3 = 0, P4 = 0;

                P1 = (rectData.y ) * iiWidth + (rectData.x);
                P2 = (rectData.y ) * iiWidth + (rectData.x + rectData.w);
                P3 = (rectData.y + rectData.h) * iiWidth + (rectData.x);
                P4 = (rectData.y + rectData.h) * iiWidth + (rectData.x + rectData.w);

                sumA = ii[P4] - ii[P3];
                sumB = ii[P1] - ii[P2];
                
                sum += (((sumA + sumB)) * rectData.weight) ;
            }
            sum = sum * 1000;

            stageTmp = (sum < nodeThresh * stddev) ? stageTmp + lval : stageTmp + rval;
        }
        //iteratedClassifer = classifierEnd;
        if (stageTmp < stageThresholds[stageIndex]) {
            return -stageIndex;
        }
    }
    return 1;
}
void Mat2Array(cv::Mat inputMat, int*& outputArray) {
    int MatHeight = inputMat.rows;
    int MatWidth = inputMat.cols;
    for (int y = 0; y < MatHeight; y++) {
        int* inputMaty = inputMat.ptr<int>(y);
        for (int x = 0; x < MatWidth; x++) {
            outputArray[(y) * (MatWidth) + (x)] = inputMaty[x];
        }
    }
}

///
void groupBoundingBoxes(std::vector<cv::Rect>& rectList, int groupThreshold, double eps) {
    //GroupBoundingBoxesAccel:
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
        //Unroll:
        int cls = labels[i];
        groupedRects[cls].x += rectList[i].x;
        groupedRects[cls].y += rectList[i].y;
        groupedRects[cls].width += rectList[i].width;
        groupedRects[cls].height += rectList[i].height;        
        groupWeights[cls]++;
    }

    
    rectList.clear();
    for (int i = 0; i < nclasses; i++) {
       // Unroll:
        if (groupWeights[i] > groupThreshold) {
            groupedRects[i].x /= groupWeights[i];
            groupedRects[i].y /= groupWeights[i];
            groupedRects[i].width /= groupWeights[i];
            groupedRects[i].height /= groupWeights[i];
            rectList.push_back(groupedRects[i]);
        }
    }
}

void convertData(const int stageRects[6383][5], int stageRect1D[]) {
    for (int i = 0; i < 6383; i++) {
        for (int j = 0; j < 5; j++) {
            stageRect1D[i * 5 + j] = stageRects[i][j];
        }
    }
}

void detectFace(cv::Mat& inputImage, float scaleFactor) {
    cv::Mat grayScaleImage = cv::Mat::zeros(inputImage.rows, inputImage.cols, CV_8UC1);
    cv::Mat integralImage, SQintegralImage, resizedImage;
    int imageHeight = inputImage.rows, imageWidth = inputImage.cols;
    float factor = 1;
    std::vector<cv::Rect> detectedRects;
    convertToGrayscale(inputImage, grayScaleImage);
    //imageScaler:
    while (imageHeight / factor > WIN_HEIGHT && imageWidth / factor > WIN_WIDTH) { //Python
        Preprocessing:
        cv::resize(grayScaleImage, resizedImage, cv::Size(), 1 / factor, 1 / factor, cv::INTER_AREA);
        createIntegralImage(resizedImage, integralImage, SQintegralImage);

        int iiHeight = integralImage.rows, iiWidth = integralImage.cols;
        int iiSqHeight = SQintegralImage.rows, iiSqWidth = SQintegralImage.cols;
        //cv::imwrite("integralImage.jpg", integralImage);

        int* ii = new int[iiHeight * iiWidth];
        int* iiSq = new int[iiSqHeight * iiSqWidth];
        //Mat2Array(integralImage, ii);
        //Mat2Array(SQintegralImage, iiSq);
        DetectFace: //(ii, iiWidth, iiHeight, iiSq, iiSqWidth, iiSqHeight)
        Point p;
        int rows = resizedImage.rows, cols = resizedImage.cols;
        //DetectorScan:
        for (int y = 0; y < rows - win_height; y++) {
            for (int x = 0; x < cols - win_width; x++) {
                p = { x, y };
                int result = cascadeClassifier(p, (int* )integralImage.data, (int*)SQintegralImage.data, iiWidth, iiHeight, iiSqWidth, iiSqHeight, factor);
                if (result > 0) {
                    cv::Rect rect(p.x * factor, p.y * factor, win_width * factor, win_height * factor);
                    detectedRects.push_back(rect);
                }
            }
        }
        factor *= scaleFactor;
        delete[] ii;
        ii = nullptr;
        delete[] iiSq;
        iiSq = nullptr;
    }

    
    groupBoundingBoxes(detectedRects, 2, 0.2); 

    
    for (const auto& rect : detectedRects) {
        cv::rectangle(inputImage, rect, cv::Scalar(0, 255, 0), 1);
    }
}

