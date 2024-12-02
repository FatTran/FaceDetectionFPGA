#include "haar.h"
#include <iostream>
#include <iomanip>

int main() {

    Rect rects[MAX_RECTS] = {
        {10, 10, 30, 30},   // Rect 1
        {15, 15, 30, 30},   // Rect 2
        {100, 100, 20, 20}, // Rect 3
        {12, 12, 35, 35},   // Rect 4
        {150, 150, 25, 25}, // Rect 5
        {110, 110, 20, 20}, // Rect 6
    };
    int numRects = 6;

    int groupThreshold = 2;
    double eps = 0.3;

    Rect groupedRects[MAX_RECTS];
    int numGrouped = 0;


    hls::stream<Rect> rectStream;


    for (int i = 0; i < numRects; ++i) {
        rectStream.write(rects[i]);
    }


    std::cout << "=== IoU Tests ===\n";
    for (int i = 0; i < numRects; ++i) {
        for (int j = i + 1; j < numRects; ++j) {
            double iou = computeIoU(rects[i], rects[j]);
            std::cout << "IoU(Rect " << i + 1 << ", Rect " << j + 1 << ") = "
                      << std::fixed << std::setprecision(2) << iou << "\n";
        }
    }


    groupBoundingBoxes(rectStream, numRects, groupThreshold, eps, rectStream, numGrouped);

    std::cout << "\n=== Grouped Bounding Boxes ===\n";
    for (int i = 0; i < numGrouped; ++i) {
        std::cout << "Group " << i + 1 << ": "
                  << "x=" << groupedRects[i].x << ", "
                  << "y=" << groupedRects[i].y << ", "
                  << "width=" << groupedRects[i].w << ", "
                  << "height=" << groupedRects[i].h << "\n";
    }

    return 0;
}
