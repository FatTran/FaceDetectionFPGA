#include "pre_processing.h"
#include "ap_int.h"

void preProcessing(
    ap_uint<INPUT_PTR_WIDTH> *input,
    ap_uint<OUTPUT_PTR_WIDTH> *output,
    int rows,
    int cols
) {
    #pragma HLS INTERFACE m_axi port=input offset=slave bundle=gmem0
    #pragma HLS INTERFACE m_axi port=output offset=slave bundle=gmem1
    #pragma HLS INTERFACE s_axilite port=rows bundle=CONTROL_BUS
    #pragma HLS INTERFACE s_axilite port=cols bundle=CONTROL_BUS
    #pragma HLS INTERFACE s_axilite port=return bundle=CONTROL_BUS
    
    int input_step = cols * 3;  
    int output_step = cols;     
    
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            #pragma HLS PIPELINE II=1
            
            int in_offset = i * input_step + j * 3;
            int out_offset = i * output_step + j;
            
            ap_uint<24> pixel = input[in_offset/4]; 
            
            uint8_t r = pixel(7,0);
            uint8_t g = pixel(15,8);
            uint8_t b = pixel(23,16);
            
            //0.299f*r + 0.587f*g + 0.114f*b 
            uint8_t gray = (r * 77 + g * 150 + b * 29) >> 8;
            
            output[out_offset] = gray;
        }
    }
}


