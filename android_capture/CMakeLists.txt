cmake_minimum_required(VERSION 3.4.3)

project(android_video_capture)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

find_package( OpenCV 3.2 REQUIRED COMPONENTS core imgproc videoio objdetect)
MESSAGE("OpenCV information:") 
MESSAGE("  OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}") 
MESSAGE("  OpenCV_LIBRARIES: ${OpenCV_LIBRARIES}") 
MESSAGE("  OpenCV_LIBRARY_DIRS: ${OpenCV_LINK_DIRECTORIES}") 

find_package( Boost 1.59.0 REQUIRED COMPONENTS program_options filesystem system)
MESSAGE("Boost information:") 
MESSAGE("  Boost_VERSION: ${Boost_VERSION}")
MESSAGE("  Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}") 
MESSAGE("  Boost_LIBRARIES: ${Boost_LIBRARIES}") 
MESSAGE("  Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}") 

set(ANDROID_CAPTURE_LIB_SRC android_capture.cpp)
set(ANDROID_CAPTURE_TEST_SRC main.cpp)

add_library(${PROJECT_NAME} ${ANDROID_CAPTURE_LIB_SRC})
add_executable(android_capture_test ${ANDROID_CAPTURE_TEST_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBRARIES})

target_include_directories(android_capture_test PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS})
target_link_libraries(android_capture_test PRIVATE ${PROJECT_NAME} ${Boost_LIBRARIES})
