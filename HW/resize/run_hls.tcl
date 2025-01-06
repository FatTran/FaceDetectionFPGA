# Copyright (C) 2019-2022, Xilinx, Inc.
# Copyright (C) 2022-2023, Advanced Micro Devices, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# vitis hls makefile-generator v2.0.0

set CSIM 1
set CSYNTH 1
set COSIM 0
set VIVADO_SYN 1
set VIVADO_IMPL 1
set CUR_DIR [pwd]
set OPENCV_INCLUDE "C:/OpenCV-4.4.0/opencv/build/install/include"
set OPENCV_LIB "C:/OpenCV-4.4.0/opencv/build/install/x64/mingw/lib"
set XF_PROJ_ROOT "C:/Vitis_Libraries/vision"
set XPART xc7z020clg400-1

set PROJ "resize.prj"
set SOLN "sol1"

if {![info exists CLKP]} {
  set CLKP 10
}

open_project -reset $PROJ

add_files "${CUR_DIR}/xf_resize_accel.cpp" -cflags " -I ${CUR_DIR}/config -I${XF_PROJ_ROOT}/L1/include -I ./ -D__SDSVHLS__ -std=c++0x" -csimflags " -I ${CUR_DIR}/config -I${XF_PROJ_ROOT}/L1/include -I ./ -D__SDSVHLS__ -std=c++0x"
add_files -tb "${CUR_DIR}/xf_resize_tb.cpp" -cflags " -I ${CUR_DIR}/config -I${OPENCV_INCLUDE} -I${XF_PROJ_ROOT}/L1/include -I ./ -D__SDSVHLS__ -std=c++0x" -csimflags " -I ${CUR_DIR}/config -I${XF_PROJ_ROOT}/L1/include -I ./ -D__SDSVHLS__ -std=c++0x"
set_top resize_accel

open_solution -reset $SOLN



set_part $XPART
create_clock -period $CLKP

if {$CSIM == 1} {
  csim_design -ldflags "-L ${OPENCV_LIB} -lopencv_imgcodecs440 -lopencv_imgproc440 -lopencv_core440 -lopencv_highgui440 -lopencv_flann440 -lopencv_features2d440" -argv " D:/UTE/Advanced_Topic/HW/img/29_gray.jpg "
}

if {$CSYNTH == 1} {
  csynth_design
}

if {$COSIM == 1} {
  cosim_design -ldflags "-L ${OPENCV_LIB} -lopencv_imgcodecs440 -lopencv_imgproc440 -lopencv_core440 -lopencv_highgui440 -lopencv_flann440 -lopencv_features2d440" -argv "  D:/UTE/Advanced_Topic/HW/img/29_gray.jpg "
}

if {$VIVADO_SYN == 1} {
  export_design -flow syn -rtl verilog
}

if {$VIVADO_IMPL == 1} {
  export_design -flow impl -rtl verilog
}

exit