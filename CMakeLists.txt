cmake_minimum_required (VERSION 3.2)
project(hiredis-camera-server)

set (CMAKE_CXX_STANDARD 11)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake/module)

find_package(OpenCV REQUIRED core opencv_imgcodecs opencv_highgui opencv_imgproc opencv_videoio)
if (NOT OPENCV_FOUND)
        message(FATAL_ERROR, "OpenCV not found.")
endif()

find_package(hiredis REQUIRED)
if (NOT HIREDIS_FOUND)
    message(FATAL_ERROR, "hiredis not found.")
endif()

find_package(ev REQUIRED)
if (NOT EV_FOUND)
    message(FATAL_ERROR, "ev not found.")
endif()

find_package(redisimagehelper REQUIRED)
if (NOT REDISIMAGEHELPER_FOUND)
	message(FATAL_ERROR, "redisimagehelper not found")
endif()

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories($(/usr/local/include/gstreamer-1.0) /gst)
include_directories(${REDISIMAGEHELPER_INCLUDE_DIR})
message("${REDISIMAGEHELPER_INCLUDE_DIR}")

set(GLOB HEADERS include/*.hpp)	

set(CLI_SOURCES
        src/client.cpp)

set(SERV_SOURCES
        src/RedisCameraServer.cpp
        src/server.cpp

        include/RedisCameraServer.hpp)

add_executable (redis-camera-client ${CLI_SOURCES} ${HEADERS})
target_link_libraries (redis-camera-client ${REDISIMAGEHELPER_LIBS} ${HIREDIS_LIBS} ${OpenCV_LIBS} ${EV_LIBS})

add_executable (redis-camera-server ${SERV_SOURCES} ${HEADERS})
target_link_libraries(redis-camera-server ${REDISIMAGEHELPER_LIBS} ${HIREDIS_LIBS} ${OpenCV_LIBS} ${EV_LIBS})
