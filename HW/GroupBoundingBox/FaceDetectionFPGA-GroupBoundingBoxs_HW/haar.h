
#include <ap_int.h>
#include <ap_fixed.h>
#include <hls_stream.h>
#include <stdint.h>
#include <algorithm>

#define MAX_RECTS 100




typedef struct Rect {
	int x;
	int y;
	int w;
	int h;
	int weight;
}Rect;


double computeIoU(const Rect& r1, const Rect& r2);

void groupBoundingBoxes(hls::stream<Rect>& rectList, int numRects, int groupThreshold, double eps, hls::stream<Rect>& groupedRects, int& numGrouped);


