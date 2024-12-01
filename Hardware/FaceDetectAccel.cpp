#include "xf_config_params.h"
// typedef struct{
//     int x;
//     int y;
//     int w;
//     int h;
//     int weight;
// }MyRect;

//sstatic constexpr int __XF_DEPTH_IN = (HEIGHT * WIDTH * (XF_PIXELWIDTH(IN_TYPE, NPPCX)) / 8) / (INPUT_PTR_WIDTH / 8);
//static constexpr int __XF_DEPTH_OUT = (HEIGHT * WIDTH * (XF_PIXELWIDTH(OUT_TYPE, NPPCX)) / 8) / (OUTPUT_PTR_WIDTH / 8);
void int_sqrt(unsigned int value, unsigned int * out_value){
#pragma INLINE
    int i;
    unsigned int a = 0, b = 0, c = 0;
    for (i = 0; i < (32 >> 1); i++)
    {
#pragma HLS UNROLL
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
    *out_value = a;
    return;
}

void RectEval(
			int x,
			int y,
			ap_int<32>* ii,
			int& rectStart,
			int& rectEnd,
			int rows,
			int cols,
			int* sum,
			ap_int<32>* stageOrga,
			ap_int<32>* stageThresholds,
			ap_int<32>* stageNodes,
			ap_int<32>* featOrga,
			ap_int<32>* stageRects,
			float factor){
	int sumA = 0;
	int sumB = 0;
	for (int rectIndex = rectStart; rectIndex < rectEnd; rectIndex++) {
#pragma HLS UNROLL
		int rectDataX = stageRects[rectIndex * 5 + 0] + x;
		int rectDataY = stageRects[rectIndex * 5 + 1] + y;
		int rectDataW = stageRects[rectIndex * 5 + 2];
		int rectDataH = stageRects[rectIndex * 5 + 3];
		int rectDataWeigth = stageRects[rectIndex * 5 + 4];

		int P1 = 0, P2 = 0, P3 = 0, P4 = 0;

		P1 = (rectDataY ) * cols + (rectDataX);
		P2 = (rectDataY ) * cols + (rectDataX + rectDataW);
		P3 = (rectDataY + rectDataH) * cols + (rectDataX);
		P4 = (rectDataY + rectDataH) * cols + (rectDataX + rectDataW);

		sumA = ii[P4] - ii[P3];
		sumB = ii[P1] - ii[P2];

		*sum += (((sumA + sumB)) * rectDataWeigth);
	}
}

void NodeEval(
		int x,
		int y,
		ap_int<32>* ii,
		int& classifierStart,
		int& classifierEnd,
		int& rectStart,
		int& rectEnd,
		int rows,
		int cols,
		int& stageTmp,
		int stddev,
		ap_int<32>* stageOrga,
		ap_int<32>* stageThresholds,
		ap_int<32>* stageNodes,
		ap_int<32>* featOrga,
		ap_int<32>* stageRects,
		float factor
){
	for (int classifierIndex = classifierStart; classifierIndex < classifierEnd; classifierIndex++) {
		//Get node data
//#pragma HLS PIPELINE ii=2
		int nodeThresh = stageNodes[classifierIndex * 3];
		int lval = stageNodes[classifierIndex * 3 + 1];
		int rval = stageNodes[classifierIndex * 3 + 2];
		rectStart = rectEnd;
		rectEnd = rectStart + featOrga[classifierIndex];
		int sum = 0;
		RectEval(
				x,
				y,
				ii,
				rectStart,
				rectEnd,
				rows,
				cols,
				&sum,
				stageOrga,
				stageThresholds,
				stageNodes,
				featOrga,
				stageRects,
				factor);
		sum = sum * 1000;
		stageTmp = (sum < nodeThresh * stddev) ? stageTmp + lval : stageTmp + rval;
	}
}

void StageEval(
		int x,
		int y,
		ap_int<32>* ii,
		int& classifierStart,
		int& classifierEnd,
		int& rectStart,
		int& rectEnd,
		int rows,
		int cols,
		int stddev,
		ap_int<32>* stageOrga,
		ap_int<32>* stageThresholds,
		ap_int<32>* stageNodes,
		ap_int<32>* featOrga,
		ap_int<32>* stageRects,
		float factor,
		int& result
){
	for (int stageIndex = 0; stageIndex < TOTAL_STAGE; stageIndex++) {
//#pragma HLS PIPELINE ii=1
		classifierStart = classifierEnd;
		classifierEnd = classifierStart + stageOrga[stageIndex];
		int stageTmp = 0;

		NodeEval(
				 x,
				 y,
				 ii,
				 classifierStart,
				 classifierEnd,
				 rectStart,
				 rectEnd,
				 rows,
				 cols,
				 stageTmp,
				 stddev,
				 stageOrga,
				 stageThresholds,
				 stageNodes,
				 featOrga,
				 stageRects,
				 factor);


		if (stageTmp < stageThresholds[stageIndex]) {
			result = 0;
			break;
		}else{
			result = 1;
		}
	}
}

void FaceDetectAccel(ap_int<32>* ii,
					ap_int<32>* iiSq,
					int rows,
					int cols,
					ap_int<32>* stageOrga,
					ap_int<32>* stageThresholds,
					ap_int<32>* stageNodes,
					ap_int<32>* featOrga,
					ap_int<32>* stageRects,
					float factor,
					ap_uint<32>* detectedX,
					ap_uint<32>* detectedY,
					ap_uint<32>* detectedW,
					ap_uint<32>* detectedH
){
#pragma HLS INTERFACE mode=m_axi port=ii offset=slave bundle=gmem0 depth=HEIGHT*WIDTH max_widen_bitwidth=512
#pragma HLS INTERFACE mode=m_axi port=iiSq offset=slave bundle=gmem1 depth=HEIGHT*WIDTH max_widen_bitwidth=512
#pragma HLS INTERFACE mode=m_axi port=stageOrga offset=slave bundle=gmem2 depth=25 max_widen_bitwidth=512
#pragma HLS INTERFACE mode=m_axi port=stageThresholds offset=slave bundle=gmem3 depth=25 max_widen_bitwidth=512
#pragma HLS INTERFACE mode=m_axi port=stageNodes offset=slave bundle=gmem4 depth=8739 max_widen_bitwidth=512
#pragma HLS INTERFACE mode=m_axi port=featOrga offset=slave bundle=gmem5 depth=2913 max_widen_bitwidth=512
#pragma HLS INTERFACE mode=m_axi port=stageRects offset=slave bundle=gmem6 depth=31915 max_widen_bitwidth=512

//#pragma HLS ARRAY_STENCIL variable=stageRects off

//#pragma HLS INTERFACE mode=m_axi port=ii offset=slave bundle=gmem0 depth=HEIGHT*WIDTH max_widen_bitwidth=512
//#pragma HLS INTERFACE mode=m_axi port=iiSq offset=slave bundle=gmem1 depth=HEIGHT*WIDTH max_widen_bitwidth=512
//#pragma HLS INTERFACE mode=m_axi port=stageOrga offset=slave bundle=gmem2 depth=1 max_widen_bitwidth=512
//#pragma HLS INTERFACE mode=m_axi port=stageThresholds offset=slave bundle=gmem3 depth=1 max_widen_bitwidth=512
//#pragma HLS INTERFACE mode=m_axi port=stageNodes offset=slave bundle=gmem4 depth=27 max_widen_bitwidth=512
//#pragma HLS INTERFACE mode=m_axi port=featOrga offset=slave bundle=gmem5 depth=9 max_widen_bitwidth=512
//#pragma HLS INTERFACE mode=m_axi port=stageRects offset=slave bundle=gmem6 depth=90 max_widen_bitwidth=512

#pragma HLS INTERFACE mode=m_axi port=detectedX offset=slave bundle=gmem7 depth=1  max_widen_bitwidth=512
#pragma HLS INTERFACE mode=m_axi port=detectedY offset=slave bundle=gmem8 depth=1 max_widen_bitwidth=512
#pragma HLS INTERFACE mode=m_axi port=detectedW offset=slave bundle=gmem9 depth=1 max_widen_bitwidth=512
#pragma HLS INTERFACE mode=m_axi port=detectedH offset=slave bundle=gmem10 depth=1 max_widen_bitwidth=512
//
#pragma HLS INTERFACE mode=s_axilite port=rows
#pragma HLS INTERFACE mode=s_axilite port=cols
#pragma HLS INTERFACE mode=s_axilite port=factor
#pragma HLS INTERFACE mode=s_axilite port=return
#pragma HLS DATAFLOW
	ScanRow:
	for(int y = 0; y < rows - WIN_HEIGHT; y++){
		ScanCol:
		for(int x = 0; x < cols - WIN_WIDTH; x++){
			int result = 0;

			int mean = 0;
			int stddev = 0;
			unsigned int stddev_sqrt = 0;

			stddev = iiSq[(y) * cols + (x)]
			    - iiSq[(y) * cols + (x + (WIN_WIDTH))]
			    - iiSq[(y + (WIN_HEIGHT)) * cols + (x)]
			    + iiSq[(y + (WIN_HEIGHT)) * cols + (x + (WIN_WIDTH))];

			mean = ii[y * cols + x]
			    - ii[y * cols + (x + (WIN_WIDTH))]
			    - ii[(y + (WIN_HEIGHT)) * cols + x]
			    + ii[(y + (WIN_HEIGHT)) * cols + (x + (WIN_WIDTH))];

			stddev = stddev * (WIN_WIDTH) * (WIN_WIDTH);
			stddev = stddev - mean * mean;
			int_sqrt(stddev, &stddev_sqrt);
			stddev = (stddev > 0) ? stddev_sqrt : 1;

			int classifierStart = 0;
			int classifierEnd = 0;
			int rectStart = 0;
			int rectEnd = 0;

			StageEval(
					 x,
					 y,
					 ii,
					 classifierStart,
					 classifierEnd,
					 rectStart,
					 rectEnd,
					 rows,
					 cols,
					 stddev,
					 stageOrga,
					 stageThresholds,
					 stageNodes,
					 featOrga,
					 stageRects,
					 factor,
					 result
					);

			//if (result > 0) {
			    *detectedX = (ap_int<32>) x * factor * result;
			    *detectedY = (ap_int<32>) y * factor * result;
			    *detectedW = (ap_int<32>) 24 * factor * result;
			    *detectedH = (ap_int<32>) 24 * factor * result;
			//}
		}
	}
	return;
}


