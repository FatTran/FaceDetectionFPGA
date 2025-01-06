#pragma once
#include "hls_stream.h"
#include "ap_int.h"
#include "common/xf_common.hpp"
#include "common/xf_utility.hpp"
#include "imgproc/xf_channel_extract.hpp"
#include "imgproc/xf_crop.hpp"
#include "imgproc/xf_custom_convolution.hpp"
#include "core/xf_arithm.hpp"
#include "imgproc/xf_channel_combine.hpp"
#include "imgproc/xf_cvt_color.hpp"
#include "imgproc/xf_histogram.hpp"
#include <cmath>
#include "imgproc/xf_convertscaleabs.hpp"
#include "imgproc/xf_lut.hpp"
#include "imgproc/xf_delay.hpp"
#include "imgproc/xf_duplicateimage.hpp"
#include "imgproc/xf_integral_image.hpp"
#ifndef __SYNTHESIS__
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#endif

#define HEIGHT 1080 //Max image height
#define WIDTH 1080 //Max image width
#define WIN_WIDTH 24
#define WIN_HEIGHT 24
#define XF_CV_DEPTH_IN_1 2
#define XF_CV_DEPTH_OUT_2 2
#define T_8U 1
#define T_16U 0
#define RGB 1
#define RGBA 0
#define NPPCX XF_NPPC1
#define IN_TYPE XF_8UC3
#define OUT_TYPE XF_8UC3
#define CV_IN_TYPE CV_8UC3
#define CV_OUT_TYPE CV_8UC3
#define INPUT_PTR_WIDTH 32 
#define OUTPUT_PTR_WIDTH 24
#define MAX_RECTS 100
#define TOTAL_STAGE 25
#define TOTAL_NODE 2913
#define TOTAL_RECT 6383



//void EvalRectAccel(
//    int detectorX,
//    int detectorY,
//    int RectStartIndex,
//    int RectEndIndex,
//    int* ii,
//    int iiWidth,
//    int iiHeight,
//    int* sum,
//    int* stageRects);
//void ImageScaler();
//
//void IntegralImageAccel();
//
//void RectEvalAccel(); //Evaluate each rectangle
//
//void NodeEvalAccel(); //Evaluate each classifier
//
//void StageEvalAccel(ap_uint<8> stageOrga,
//					ap_int<16> stageThresholds,
//					ap_int<16> stageNodes,
//					ap_uint<8> featOrga,
//					ap_int<8> stageRects,
//					int classifierStart,
//					int classifierEnd,
//
//					); //Evaluate each stage
//void RGB2Gray(
//    ap_uint<24> *input,
//    ap_uint<8> *output,
//    int rows,
//    int cols
//);
void int_sqrt(unsigned int value, unsigned int * out_value);
void FaceDetect(ap_uint<32>* ii,
					ap_uint<32>* iiSq,
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
					ap_uint<32>* detectedH); //Cascade classifier evaluation
//void FaceDetectAccel(int* ii,
//					int* iiSq,
//					int rows,
//					int cols,
//					const int* stageOrga,
//					const int* stageThresholds,
//					const int* stageNodes,
//					const int* featOrga,
//					const int* stageRects,
//					float factor,
//					int* detectedX,
//					int* detectedY,
//					int* detectedW,
//					int* detectedH
//);

//void FaceDetectAccel(ap_uint<24>* in_img,
//					 ap_uint<8>* stageOrga,
//					 ap_int<16>* stageThresholds,
//					 ap_int<16>* stageNodes,
//					 ap_uint<8>* featOrga,
//					 ap_int<8>* stageRects,
//					 float scaleFactor,
//					 ap_uint<32>* detectedX,
//					 ap_uint<32>* detectedY,
//					 ap_uint<32>* detectedW,
//					 ap_uint<32>* detectedH
//					);

/*unsigned int int_sqrt(unsigned int value);*/

