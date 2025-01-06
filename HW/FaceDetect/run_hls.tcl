set CSIM 1
set CSYNTH 1
set COSIM 1
set VIVADO_SYN 1
set VIVADO_IMPL 1
set CUR_DIR [pwd]
set OPENCV_INCLUDE "C:/OpenCV-4.4.0/opencv/build/install/include"
set OPENCV_LIB "C:/OpenCV-4.4.0/opencv/build/install/x64/mingw/lib"
set XF_PROJ_ROOT "C:/Vitis_Libraries/vision"
set XPART xc7z020clg400-1
set PROJ "face_detect.prj"
set SOLN "sol1"

if {![info exists CLKP]} {
    set CLKP 10
}
# -DALLOW_EMPTY_HLS_STREAM_READS
open_project -reset $PROJ
add_files "${CUR_DIR}/FaceDetectAccel.cpp" -cflags "-I ${OPENCV_INCLUDE} -I ${CUR_DIR} -I ${CUR_DIR}/config -I${XF_PROJ_ROOT}/L1/include -I ./ -D__SDSVHLS__ -std=c++14" -csimflags "-DALLOW_EMPTY_HLS_STREAM_READS -I ${OPENCV_INCLUDE} -I ${CUR_DIR}/config -I${XF_PROJ_ROOT}/L1/include -I ./ -D__SDSVHLS__ -std=c++17"


add_files -tb "${CUR_DIR}/FaceDetectAccel_tb.cpp" -cflags "-I ${OPENCV_INCLUDE} -I ${CUR_DIR} -I ${CUR_DIR}/config -I${XF_PROJ_ROOT}/L1/include -I ./ -D__SDSVHLS__ -std=c++14" -csimflags "-DALLOW_EMPTY_HLS_STREAM_READS -I ${OPENCV_INCLUDE} -I ${CUR_DIR}/config -I${XF_PROJ_ROOT}/L1/include -I ./ -D__SDSVHLS__ -std=c++17"

set_top FaceDetect
open_solution -reset $SOLN
config_interface -m_axi_addr64=true
config_interface -m_axi_max_widen_bitwidth 128
# config_dataflow -default_channel fifo -fifo_depth 200000

config_interface -m_axi_alignment_byte_size 64
set_part $XPART
create_clock -period $CLKP
if {$CSIM == 1} {
    csim_design -ldflags "-L ${OPENCV_LIB} -lopencv_imgcodecs440 -lopencv_imgproc440 -lopencv_core440 -lopencv_highgui440 -lopencv_flann440 -lopencv_features2d440"
}

if {$CSYNTH == 1} {
    csynth_design
}

if {$COSIM == 1} {
    cosim_design -ldflags "-L ${OPENCV_LIB} -lopencv_imgcodecs440 -lopencv_imgproc440 -lopencv_core440 -lopencv_highgui440 -lopencv_flann440 -lopencv_features2d440"
}

if {$VIVADO_SYN == 1} {
    export_design -flow syn -rtl verilog
}

if {$VIVADO_IMPL == 1} {
    export_design -flow impl -rtl verilog
}

exit