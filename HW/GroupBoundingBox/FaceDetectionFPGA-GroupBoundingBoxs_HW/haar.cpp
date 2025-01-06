#include "haar.h"
//unsigned int int_sqrt(ap_uint<32> value) {
//    int i;
//    unsigned int a = 0, b = 0, c = 0;
//
//    for (i = 0; i < (32 >> 1); i++) {
//		#pragma HLS unroll
//        c <<= 2;
//        #define UPPERBITS(value) (value >> 30)
//        c += UPPERBITS(value);
//        #undef UPPERBITS
//        value <<= 2;
//        a <<= 1;
//        b = (a << 1) | 1;
//        if (c >= b) {
//            c -= b;
//            a++;
//        }
//    }
//    return a;
//}
//int evalRect(Rect detector, int rectStart, int rectEnd, int* ii, int iiWidth, const int stageRects[6383][5]) {
//	#pragma HLS ARRAY_PARTITION variable=stageRects dim=2 complete
//    #pragma HLS INLINE
//    int sum = 0;
//    int sumA = 0, sumB = 0;
//    for (int rectIndex = rectStart; rectIndex < rectEnd; rectIndex++) {
//		#pragma HLS PIPELINE II=1
//		//#pragma HLS ARRAY_PARTITION
//        Rect rectData = {
//            stageRects[rectIndex][0] + detector.x,
//            stageRects[rectIndex][1] + detector.y,
//            stageRects[rectIndex][2],
//            stageRects[rectIndex][3],
//            stageRects[rectIndex][4]
//        };
//
//        int P1 = 0, P2 = 0, P3 = 0, P4 = 0;
//
//        P1 = (rectData.y - 1) * iiWidth + (rectData.x - 1);
//        P2 = (rectData.y - 1) * iiWidth + (rectData.x - 1 + rectData.w);
//        P3 = (rectData.y - 1 + rectData.h) * iiWidth + (rectData.x - 1);
//        P4 = (rectData.y - 1 + rectData.h) * iiWidth + (rectData.x - 1 + rectData.w);
//
//        if (rectData.x == 0)
//        {
//            sumA = ii[P4];
//            sumB = (rectData.y == 0) ? 0 : -ii[P2];
//        }
//        else
//        {
//        	sumA = ii[P4] - ii[P3];
//            sumB = (rectData.y == 0) ? 0 : ii[P1] - ii[P2];
//        }
//        sum += (((sumA + sumB)) * rectData.weight) ;
//    }
//    return sum * 1000;
//}
//
//int evalNode(Rect detector, int classifierStart, int classifierEnd, int* ii, int iiWidth, const int stageRects[6383][5], const int* stageNodes, int* featOrga, int stddev, int& iteratedRect) {
//	//#pragma HLS INLINE
//	int stageTmp = 0;
//    for (int classifierIndex = classifierStart; classifierIndex < classifierEnd; classifierIndex++) {
//        int nodeThresh = stageNodes[classifierIndex * 3];
//        int lval = stageNodes[classifierIndex * 3 + 1];
//        int rval = stageNodes[classifierIndex * 3 + 2];
//
//        int rectStart = iteratedRect;
//        int rectEnd = rectStart + featOrga[classifierIndex];
//        int sum = evalRect(detector, rectStart, rectEnd, ii, iiWidth, stageRects);
//
//        iteratedRect += featOrga[classifierIndex];
//        if (sum < nodeThresh * stddev) {
//            stageTmp += lval;
//        } else {
//            stageTmp += rval;
//        }
//    }
//    return stageTmp;
//}
//
//int evalStage(Point p, int* ii, int* iiSq, int iiWidth, int iiHeight, int iiSqWidth, int iiSqHeight, int stddev, int& iteratedClassifer, int& iteratedRect, float factor) {
//    //#pragma HLS INLINE
//	Rect detector = { p.x, p.y, (int)win_width, (int)win_height, 0 };
//    for (int stageIndex = 0; stageIndex < TOTAL_STAGE; stageIndex++) {
//        int classifierStart = iteratedClassifer;
//        int classifierEnd = classifierStart + stageOrga[stageIndex];
//        int stageTmp = evalNode(detector, classifierStart, classifierEnd, ii, iiWidth, stageRects, stageNodes, featOrga, stddev, iteratedRect);
//        iteratedClassifer += stageOrga[stageIndex];
//        if (stageTmp < stageThresholds[stageIndex]) {
//            return -stageIndex;
//        }
//    }
//    return 1;
//}
//
//int cascadeClassifier(Point p, int* ii, int* iiSq, int iiWidth, int iiHeight, int iiSqWidth, int iiSqHeight, float factor) {
//    //#pragma HLS DATAFLOW
//	int iteratedClassifer = 0, iteratedRect = 0;
//    int mean = ii[p.y * iiWidth + p.x] - ii[p.y * iiWidth + (p.x + (win_width))]
//               - ii[(p.y + (win_height)) * iiWidth + p.x]
//               + ii[(p.y + (win_height)) * iiWidth + (p.x + (win_width))];
//    int stddev = iiSq[(p.y) * iiSqWidth + (p.x)]
//                 - iiSq[(p.y) * iiSqWidth + (p.x + (win_width))]
//                 - iiSq[(p.y + (win_height)) * iiSqWidth + (p.x)]
//                 + iiSq[(p.y + (win_height)) * iiSqWidth + (p.x + (win_width))];
//    stddev = stddev * (win_width) * (win_width) - mean * mean;
//    stddev = (stddev > 0) ? int_sqrt(stddev) : 1;
//
//    return evalStage(p, ii, iiSq, iiWidth, iiHeight, iiSqWidth, iiSqHeight, stddev, iteratedClassifer, iteratedRect, factor);
//}

////////////////////////////////////////////////GROUP_RECTANGLE//////////////////////////////////////////////////

double computeIoU(const Rect& r1, const Rect& r2) {
	#pragma HLS INLINE
    int x1 = std::max(r1.x, r2.x);
    int y1 = std::max(r1.y, r2.y);
    int x2 = std::min(r1.x + r1.w, r2.x + r2.w);
    int y2 = std::min(r1.y + r1.h, r2.y + r2.h);

    int intersectionArea = std::max(0, x2 - x1) * std::max(0, y2 - y1);
    int unionArea = r1.w * r1.h + r2.w * r2.h - intersectionArea;

    return static_cast<double>(intersectionArea) / unionArea;
}

//typedef hls::stream<Rect> RectStream;

void groupBoundingBoxes(hls::stream<Rect>& rectList, int numRects, int groupThreshold, double eps, hls::stream<Rect>& groupedRects, int& numGrouped) {
	//#pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS INTERFACE axis port=rectList
    #pragma HLS INTERFACE axis port=groupedRects
    #pragma HLS INTERFACE s_axilite port=numRects
    #pragma HLS INTERFACE s_axilite port=groupThreshold
    #pragma HLS INTERFACE s_axilite port=eps
    #pragma HLS INTERFACE s_axilite port=numGrouped
	#pragma HLS PIPELINE II=1
    bool merged[MAX_RECTS] = {false};
    numGrouped = 0;

    Rect localRects[MAX_RECTS];

    for (int i = 0; i < numRects; ++i) {
        #pragma HLS PIPELINE II=1
		#pragma HLS LOOP_TRIPCOUNT min=1 max=MAX_RECTS
        if (!rectList.empty()) {
            localRects[i] = rectList.read();
        }
    }

    for (int i = 0; i < numRects; ++i) {
        #pragma HLS PIPELINE II=1
        if (merged[i]) continue;

        Rect group[MAX_RECTS];
        int groupCount = 0;
        group[groupCount++] = localRects[i];
        merged[i] = true;

        for (int j = 0; j < numRects; ++j) {
            #pragma HLS PIPELINE II=1
			#pragma HLS LOOP_TRIPCOUNT min=1 max=MAX_RECTS
            if (i == j || merged[j]) continue;

            double iou = computeIoU(localRects[i], localRects[j]);
            if (iou >= eps) {
                group[groupCount++] = localRects[j];
                merged[j] = true;
            }
        }

        if (groupCount >= groupThreshold) {
            int totalWeight = 0;
            int sumX = 0, sumY = 0, sumWidth = 0, sumHeight = 0;

            for (int k = 0; k < groupCount; ++k) {
                #pragma HLS UNROLL factor=4
                int weight = group[k].w * group[k].h;
                totalWeight += weight;
                sumX += group[k].x * weight;
                sumY += group[k].y * weight;
                sumWidth += group[k].w * weight;
                sumHeight += group[k].h * weight;
            }

            Rect weightedRect;
            weightedRect.x = sumX / totalWeight;
            weightedRect.y = sumY / totalWeight;
            weightedRect.w = sumWidth / totalWeight;
            weightedRect.h = sumHeight / totalWeight;

            groupedRects.write(weightedRect);
            numGrouped++;
        }
    }
}


