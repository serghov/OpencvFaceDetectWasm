cmake_minimum_required(VERSION 3.6)
project(opencvWasmFaceDetect)

set(CMAKE_CXX_COMPILER "/home/serg/build/emsdk/emscripten/1.37.14/emcc")

set(CMAKE_CXX_STANDARD 11)

if (DEFINED ENV{OPENCV_INSTALL_DIR})
    set(OPENCV_DIR  $ENV{OPENCV_INSTALL_DIR})
else()
    set(OPENCV_DIR "/home/serg/build/opencv/build/install")
endif()



if (DEFINED ENV{EMSCRIPTEN})
    set(EMSCRIPTEN_DIR $ENV{EMSCRIPTEN})
else()
    set(EMSCRIPTEN_DIR "/home/serg/build/emsdk/emscripten/1.37.14")
endif()

include_directories(
        "${OPENCV_DIR}/include"
        "${EMSDK_DIR}/system/include"
)
link_directories("${OPENCV_DIR}/lib")

set (EXPORTED_FUNCTIONS "'_onNewImage', '_main'")

set(SOURCE_FILES cpp/main.cpp)
set(CMAKE_CXX_FLAGS " -O3 --preload-file filesystem@/ -s TOTAL_MEMORY=134217728 -s STB_IMAGE=1 -s WASM=1  -s ASSERTIONS=1 -s EXPORTED_FUNCTIONS=\"[${EXPORTED_FUNCTIONS}]\" ")

set(TARGET_NAME "opencvWasmFaceDetect.js")

add_executable(${TARGET_NAME} ${SOURCE_FILES})

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/web $<TARGET_FILE_DIR:${TARGET_NAME}>)

add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/filesystem $<TARGET_FILE_DIR:${TARGET_NAME}>/filesystem)



target_link_libraries(${TARGET_NAME}
        libopencv_core
        libopencv_imgproc
        libopencv_imgcodecs
        libopencv_highgui
        libopencv_photo
        libopencv_video
        libopencv_videoio
        libopencv_objdetect)
