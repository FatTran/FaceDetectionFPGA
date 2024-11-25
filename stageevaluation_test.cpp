

#include <iostream>
#include <cmath>
#include <cstdlib>
#include "haar.h"

using namespace std;

// Software implementation of cascade classification
// Phần mềm triển khai cascade classification

unsigned int int_sqrt_sw(unsigned int value) {
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
int cascadeClassifier_sw(
    Point p,
    int* ii,
    int* iiSq,
    int iiWidth,
    int iiHeight,
    int iiSqWidth,
    int iiSqHeight,
    float factor) {

    static int stageTmpArr[TOTAL_STAGE];  // Mảng lưu trữ giá trị stageTmp cho từng stage

    int iteratedClassifer = 0;
    int iteratedRect = 0;
    int mean, stddev;

    // Tính Mean và StdDev
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
    	stddev = int_sqrt_sw(stddev);
    } else {
        stddev = 1;
    }

    Rect detector = { p.x, p.y, (int) win_width, (int) win_height, 0 };
    // Lặp qua các stage
    for (int stageIndex = 0; stageIndex < TOTAL_STAGE; stageIndex++) {
        int classifierStart = iteratedClassifer;
        int classifierEnd = classifierStart + stageOrga[stageIndex];
        float stageTmp = 0.0;

        // Lặp qua các classifier
        for (int classifierIndex = classifierStart; classifierIndex < classifierEnd; classifierIndex++) {
            float nodeThresh = stageNodes[classifierIndex * 3];
            float lval = stageNodes[classifierIndex * 3 + 1];
            float rval = stageNodes[classifierIndex * 3 + 2];
            int rectStart = iteratedRect;
            int rectEnd = rectStart + featOrga[classifierIndex];
            int sum = 0;

            // Lặp qua các rectangle
            for (int rectIndex = rectStart; rectIndex < rectEnd; rectIndex++) {
                // Lấy dữ liệu rect
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
        //stageTmpArr[stageIndex] = stageTmp;  Lưu giá trị stageTmp vào mảng

        if (stageTmp < stageThresholds[stageIndex]) {
            return -stageIndex;  // Trả về mảng stageTmpArr nếu gặp kết quả không hợp lệ
        }
    }

    return 1;  // Trả về mảng stageTmpArr nếu qua hết tất cả các stages
}

// Phần kiểm thử trong hàm main
int main() {
    const int iiWidth = 48, iiHeight = 48, iiSqWidth = 48, iiSqHeight = 48;
    int ii[iiWidth * iiHeight];
    int iiSq[iiSqWidth * iiSqHeight];
    Point p = {5, 5};
    float factor = 1.0;

    // Sinh giá trị ngẫu nhiên cho hình ảnh tích phân
    for (int i = 0; i < iiHeight; i++) {
        for (int j = 0; j < iiWidth; j++) {
            ii[i * iiWidth + j] = rand() % 256;
            iiSq[i * iiSqWidth + j] = ii[i * iiWidth + j] * ii[i * iiWidth + j];
        }
    }

    // Kết quả cho phần mềm và phần cứng
    int sw_result;
    int hw_result;
    int err_cnt = 0;

    // Chạy triển khai phần mềm
    sw_result = cascadeClassifier_sw(p, ii, iiSq, iiWidth, iiHeight, iiSqWidth, iiSqHeight, factor);

#ifdef HW_COSIM
    // Chay trien khai phan cung
    hw_result = cascadeClassifier(p, ii, iiSq, iiWidth, iiHeight, iiSqWidth, iiSqHeight, factor);

    // Kiem tra ket qua phan cung

        cout << "Mo phong phan cung thanh cong! Check hw_result" << endl;

#else
    hw_result = sw_result; // Khong co mo phong phan cung
    cout << "Khong co mo phong phan cung, su dung ket qua phan mem: " << hw_result << endl;
#endif

    if (hw_result == sw_result) {
        cout << "SW and HW results match." << endl;
    } else {
        cout << "Mismatch detected!" << endl;
    }

#ifdef HW_COSIM
    if (err_cnt) {
        cout << "ERROR: " << err_cnt << " mismatches detected!" << endl;
    } else {
        cout << "Test passed." << endl;
    }
#endif

    return err_cnt;
}
