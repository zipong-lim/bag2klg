cmake_minimum_required(VERSION 2.6)

project(bag2klg)

set(CMAKE_CXX_FLAGS "-std=c++11 -O3")

include(FindPackageHandleStandardArgs)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

macro(CANONIFY_BOOL var)
  if (${var})
    set(${var} TRUE)
  else()
    set(${var} FALSE)
  endif()
endmacro()

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(RealSense2 REQUIRED)

find_package(RealSense2)
CANONIFY_BOOL(REALSENSE_FOUND)
message(STATUS "librealsense found: ${REALSENSE_FOUND}")
option(WITH_REALSENSE "Build with librealsense support?" ${REALSENSE_FOUND})
if(WITH_REALSENSE)
  add_definitions(-DWITH_REALSENSE)
  set(EXTRA_INCL ${EXTRA_INCL} ${REALSENSE_INCLUDE_DIR})
  set(EXTRA_LIBS ${EXTRA_LIBS} ${REALSENSE_LIBRARY})
endif()

find_package(ZLIB)
CANONIFY_BOOL(ZLIB_FOUND)
message(STATUS "zlib found: ${ZLIB_FOUND}")
option(WITH_ZLIB "Build with zlib support? (strongly recommended)" ${ZLIB_FOUND})
if(WITH_ZLIB)
  add_definitions(-DWITH_ZLIB)
  set(EXTRA_INCL ${EXTRA_INCL} ${ZLIB_INCLUDE_DIR})
  set(EXTRA_LIBS ${EXTRA_LIBS} ${ZLIB_LIBRARY})
endif()

find_package(JPEG)
CANONIFY_BOOL(JPEG_FOUND)
message(STATUS "libjpeg found: ${JPEG_FOUND}")
option(WITH_JPEG "Build with libjpeg support? (strongly recommended)" ${JPEG_FOUND})
if(WITH_JPEG)
  add_definitions(-DWITH_JPEG)
  set(EXTRA_INCL ${EXTRA_INCL} ${JPEG_INCLUDE_DIR})
  set(EXTRA_LIBS ${EXTRA_LIBS} ${JPEG_LIBRARY})
endif()

include_directories(
    ${GLUT_INCLUDE_DIR}
	${EXTRA_INCL}
)
  
add_executable(bag2klg 
				main.cpp
)

target_link_libraries(bag2klg
					  ${GLUT_LIBRARY}
					  ${EXTRA_LIBS}
)

install(TARGETS bag2klg 
		DESTINATION bin)
