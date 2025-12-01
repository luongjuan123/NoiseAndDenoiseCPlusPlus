cmake_minimum_required(VERSION 3.10)
project(ImageProcessingApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
find_package(OpenCV REQUIRED)

add_executable(ImageProcessingApp
    main.cpp
    imageprocessing.cpp
    imageprocessing.h
)

target_include_directories(ImageProcessingApp PRIVATE ${OpenCV_INCLUDE_DIRS})

# [SỬA] Bỏ Qt5::Charts
target_link_libraries(ImageProcessingApp PRIVATE Qt5::Widgets Qt5::PrintSupport ${OpenCV_LIBS})