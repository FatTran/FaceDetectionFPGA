#include "Integral_image.h"

void integralImage (
    ap_uint<8> *input_array,
    ap_uint<32> *output_array,
    ap_uint<32> *sq_output_array,
    int rows,
    int cols
)
{
  int u,v;
  int i,j;
  int y,x;
  int k;

    /** Image Line buffer ( 24 BRAMs ) */ 
  unsigned char L[WINDOW_SIZE-1][IMAGE_WIDTH];
  #pragma HLS array_partition variable=L complete dim=1

  /** Image Window buffer ( 1250 registers )*/
  static int_I I[WINDOW_SIZE][2*WINDOW_SIZE];
  #pragma HLS array_partition variable=I complete dim=0

  /** Integral Image Window buffer ( 625 registers )*/
  static int_II II[WINDOW_SIZE][WINDOW_SIZE];
  #pragma HLS array_partition variable=II complete dim=0
  
  /** Square Image Window buffer ( 1250 registers ) **/
  static int_SI SI[WINDOW_SIZE][2*WINDOW_SIZE];
  #pragma HLS array_partition variable=SI complete dim=0

  /** Square Integral Image Window buffer ( 625 registers )*/
  static int_SII SII[WINDOW_SIZE][WINDOW_SIZE];
  #pragma HLS array_partition variable=SII complete dim=0

    // Initialize for image line buffer
    for (int y = 0; y < WINDOW_SIZE - 1; y++) {
    #pragma HLS unroll
        for (int x = 0; x < IMAGE_WIDTH; x++) {
            L[y][x] = 0;
        }
    }

    // Initialize for (square)image window buffer
    for (int i = 0; i < WINDOW_SIZE; i++) {
    #pragma HLS unroll
        for (int j = 0; j < 2*WINDOW_SIZE; j++) {
        #pragma HLS unroll
            I[i][j] = 0;
            SI[i][j] = 0;
        }
    }

    // Initialize for Integral image window buffer
    for (int u = 0; u < WINDOW_SIZE; u++) {
    #pragma HLS unroll
        for (int v = 0; v < WINDOW_SIZE; v++) {
        #pragma HLS unroll
            II[u][v] = 0;
            SII[u][v] = 0;
        }
    }

    pixel_rows: for (y = 0; y < rows ; y++){
        pixel_cols: for (x = 0; x < cols; x++){

            Update_I: for (i = 0; i < WINDOW_SIZE; i++){
                       for (j = 0; j < 2*WINDOW_SIZE; j++){
                        if (i + j != 2*WINDOW_SIZE - 1){
                            I[i][j] = I[i][j+1];
                            SI[i][j] = SI[i][j+1];
                        }
                        else if ( i > 0){
                            I[i][j] = I[i][j+1] + I[i-1][j+1];
                            SI[i][j] = SI[i][j+1] + SI[i-1][j+1];
                        }
                       }
                    }       
            last_col: for( i = 0; i < WINDOW_SIZE-1; i++ ){
                    I[i][2*WINDOW_SIZE-1] = L[i][x];
                    SI[i][2*WINDOW_SIZE-1] = L[i][x]*L[i][x];
                    }

            Update_II: for (u = 0; u < WINDOW_SIZE; u++){
                        for (v = 0; v < WINDOW_SIZE; v++){
                            II[u][v] = I[u][v] + II[u][v-1] + II[u-1][v] - II[u-1][v-1];
                            SII[u][v] = I[u][v]*I[u][v] + SII[u][v-1] + SII[u-1][v] - SII[u-1][v-1];
                        }
            }

            // Input new pixel
            ap_uint<8> pixel = input_array[y * cols + x];
            I[WINDOW_SIZE-1][2*WINDOW_SIZE-1] = pixel;
            SI[WINDOW_SIZE-1][2*WINDOW_SIZE-1] = pixel * pixel;

            // Line buffer
            for( k = 0; k < WINDOW_SIZE-2; k++ ){
            L[k][x] = L[k+1][x];
            }
            L[WINDOW_SIZE-2][x] = pixel;

            // Write output 
            if (y >= WINDOW_SIZE-1 && x >= WINDOW_SIZE-1) {
                int out_idx = y * cols + x;
                output_array[out_idx] = II[WINDOW_SIZE-1][WINDOW_SIZE-1];
                sq_output_array[out_idx] = SII[WINDOW_SIZE-1][WINDOW_SIZE-1];
            }
        }
    }
}

