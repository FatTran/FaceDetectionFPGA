#include "haar.h"
#include <hls_math.h>
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
int cascadeClassifier(
    Point p,
    int* ii,
    int* iiSq,
    int iiWidth,
    int iiHeight,
    int iiSqWidth,
    int iiSqHeight,
    float factor
) {
#pragma HLS INLINE
    int iteratedClassifer = 0;
    int iteratedRect = 0;
    int mean, stddev;

    // Allocate an array to store stageTmp values for each stage
    static int stageTmpArr[TOTAL_STAGE];

    // Tinh toan Mean va StdDev tu integral image
    stddev = iiSq[(p.y) * iiSqWidth + (p.x)]
           - iiSq[(p.y) * iiSqWidth + (p.x + win_width)]
           - iiSq[(p.y + (win_height)) * iiSqWidth + (p.x)]
           + iiSq[(p.y + (win_height)) * iiSqWidth + (p.x + win_width)];

    mean = ii[p.y * iiWidth + p.x]
         - ii[p.y * iiWidth + (p.x + win_width)]
         - ii[(p.y + win_height) * iiWidth + p.x]
         + ii[(p.y + win_height) * iiWidth + (p.x + win_width)];

    stddev = stddev * (win_width) * (win_width) - mean * mean;

    if (stddev > 0) {
    	stddev = int_sqrt(stddev);
    } else {
        stddev = 1;
    }

    Rect detector = { p.x, p.y, (int)win_width, (int)win_height, 0 };

    // Vong lap qua cac stages
    for (int stageIndex = 0; stageIndex < TOTAL_STAGE; stageIndex++) {
#pragma HLS PIPELINE II=1
        int classifierStart = iteratedClassifer;
        int classifierEnd = classifierStart + stageOrga[stageIndex];
        float stageTmp = 0.0;

        // Vong lap qua cac classifier
        for (int classifierIndex = classifierStart; classifierIndex < classifierEnd; classifierIndex++) {
            float nodeThresh = stageNodes[classifierIndex * 3];
            float lval = stageNodes[classifierIndex * 3 + 1];
            float rval = stageNodes[classifierIndex * 3 + 2];
            int rectStart = iteratedRect;
            int rectEnd = rectStart + featOrga[classifierIndex];
            int sum = 0;

            // Vong lap qua cac hinh chu nhat (features)
            for (int rectIndex = rectStart; rectIndex < rectEnd; rectIndex++) {
#pragma HLS UNROLL
            	 //Get rectData
            	 Rect rectData = {
            	     stageRects[rectIndex][0] + detector.x,
            	     stageRects[rectIndex][1] + detector.y,
            	     stageRects[rectIndex][2],
            	     stageRects[rectIndex][3],
            	     stageRects[rectIndex][4]
            	 };
                int P1 = (rectData.y - 1) * iiWidth + (rectData.x - 1);
                int P2 = (rectData.y - 1) * iiWidth + (rectData.x - 1 + rectData.w);
                int P3 = (rectData.y - 1 + rectData.h) * iiWidth + (rectData.x - 1);
                int P4 = (rectData.y - 1 + rectData.h) * iiWidth + (rectData.x - 1 + rectData.w);

                int sumTemp = ii[P4] - ii[P3] - ii[P2] + ii[P1];
                sum += sumTemp * rectData.weight;
            }
            sum = sum * 1000;

            iteratedRect += featOrga[classifierIndex];
            if (sum < nodeThresh * stddev) {
                stageTmp += lval;
            } else {
                stageTmp += rval;
            }
        }

        iteratedClassifer += stageOrga[stageIndex];
        // stageTmpArr[stageIndex] = stageTmp;   Store stageTmp for the current stage

        if (stageTmp < stageThresholds[stageIndex]) {
            return -stageIndex;  // Return early if stageTmp < threshold
        }
    }

    return 1 ;
}
