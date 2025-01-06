cmake_minimum_required(VERSION 3.0.0)
project(FaceDetection)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(FaceDetection face_detection.cpp extractedData/haarcascade_frontalface_default.h)

target_link_libraries(FaceDetection ${OpenCV_LIBS})