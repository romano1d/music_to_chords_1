cmake_minimum_required(VERSION 3.10)
project(ChORdS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(MINGW_PATH "C:/Users/Roman/INFA/ucrt64")
include_directories(
    ${MINGW_PATH}/include
    "C:/Users/Roman/Desktop/game"
)
link_directories(${MINGW_PATH}/lib)


add_executable(furi furi.cpp)

target_link_libraries(furi PRIVATE fftw3 sndfile)