#include <iostream>
#include <memory>
#include <string>
#include <cstdint>
#include <algorithm>
#include <map>
#include <atomic>
#include <cstring>
#include <functional>
#include <fstream>

#include <chrono>
#include <thread>

#include "GL/glut.h"
#include "GL/freeglut.h"

#include "librealsense2/rs.hpp"

const static int depthWidth = 640;
const static int depthHeight = 480;
const static int rgbWidth = 640;
const static int rgbHeight = 480;
const static int overrideFps = 30;

const static int DISPLAY_MODE = 1;
const static int WRITE_MODE = 1;
const static int OVERRIDE_TIMESTAMP = 0;

const static std::string fileName = "skateboard_640x480.bag";

int main(int argc,char *argv[]){
	using namespace std::this_thread;
	using namespace std::chrono;

	// load .bag file
	rs2::config cfg;
	cfg.enable_device_from_file(fileName, 0);
	rs2::pipeline pipe;
	pipe.start(cfg);
	
	// get device depth scale
	rs2::device device = pipe.get_active_profile().get_device();
	float depthScale = device.first<rs2::depth_sensor>().get_depth_scale();;

	// initialize buffer
	uint8_t * newDepth = (uint8_t *)calloc(depthWidth * depthHeight * 2,sizeof(uint8_t));
	uint8_t * newImage = (uint8_t *)calloc(rgbWidth * rgbHeight * 3,sizeof(uint8_t));
	std::pair<std::pair<uint8_t *,uint8_t *>,int64_t> frameBuffer = std::pair<std::pair<uint8_t *,uint8_t *>,int64_t>(std::pair<uint8_t *,uint8_t *>(newDepth,newImage),0);
	std::pair<uint8_t *,int64_t> rgbBuffer = std::pair<uint8_t *,int64_t>(newImage,0);

	// initialize openGL display
	if (DISPLAY_MODE) {
		glutInit(&argc,argv);
		glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
		glutInitWindowSize(depthWidth+rgbWidth,std::max(depthHeight,rgbHeight));
		glutCreateWindow("RGBoutput"); 
	}

	// initialize file write
	int32_t nFrames = 0;
	std::ofstream file;
	if (WRITE_MODE) {
		std::string outDir = "";
		std::string fn = outDir + "output.klg";
		file.open(fn,std::ios::binary);
		if(!file.is_open()){
			std::cout << "Unable to open output file.";
			exit(0);
		}
		file.write((char*)&nFrames,sizeof(int32_t));
	}

	// variables initialization
	int32_t depthSize = depthWidth * depthHeight * sizeof(uint16_t);
	const void *depthData;
	int32_t rgbSize = rgbWidth * rgbHeight * 3 * sizeof(uint8_t);
	const void *rgbData;
	int64_t timestamp;
	int64_t currTimestamp;
	rs2::align align(RS2_STREAM_COLOR);
	rs2::frameset fs;
	int overrideFps = 30;
	int delay = 1000 / overrideFps;

	try {
		while (fs = pipe.wait_for_frames()) {
			
			if (OVERRIDE_TIMESTAMP) {
				// manual timestamp
				timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 1000000;
				std::cout << "timestamp:" << timestamp << std::endl;
			} else {
				// read timestamp from .bag file
				timestamp = fs.get_color_frame().get_timestamp();
				timestamp = timestamp % 1000000;
			}
			std::cout << "timestamp:" << timestamp << std::endl;

			// align frames
			auto aligned_fs = align.process(fs);

			// get rgb and depth frames into buffer
			// auto color_frame = fs.get_color_frame();
			auto color_frame = aligned_fs.get_color_frame();
			memcpy(rgbBuffer.first, color_frame.get_data(), color_frame.get_width() * color_frame.get_height() * 3);
			rgbBuffer.second = timestamp;

			// auto depth_frame = fs.get_depth_frame();
			auto depth_frame = aligned_fs.get_depth_frame();
			memcpy(frameBuffer.first.first,depth_frame.get_data(), depth_frame.get_width() * depth_frame.get_height() * 2);
			memcpy(frameBuffer.first.second,rgbBuffer.first, rgbWidth * rgbHeight * 3);
			frameBuffer.second = timestamp;

			void *depthData = frameBuffer.first.first;
			void *rgbData = frameBuffer.first.second;

			// display recording
			if (DISPLAY_MODE) {
				glClear(GL_COLOR_BUFFER_BIT);
				glPixelZoom(1,-1);

				glRasterPos2f(-1,1);
				glPixelTransferf(GL_RED_SCALE,0xFFFF * depthScale / 3.0f);
				glDrawPixels(depthWidth,depthHeight,GL_RED,GL_UNSIGNED_SHORT,depthData);
				glPixelTransferf(GL_RED_SCALE,1.0f);

				glRasterPos2f(0,1);
				glDrawPixels(rgbWidth,rgbHeight,GL_RGB,GL_UNSIGNED_BYTE,rgbData);
				glFlush(); 
			}

			depthData = frameBuffer.first.first;
			rgbData = frameBuffer.first.second;
			currTimestamp = frameBuffer.second;

			// save data into .klg
			if (WRITE_MODE) {
				file.write((char*)&currTimestamp,sizeof(int64_t));
				file.write((char*)&depthSize,sizeof(int32_t));
				file.write((char*)&rgbSize,sizeof(int32_t));
				file.write((char*)depthData,depthSize);
				file.write((char*)rgbData,rgbSize);
			}

			nFrames++;
			if (OVERRIDE_TIMESTAMP) sleep_for(nanoseconds(delay));
		}
	}
	catch (const rs2::error &e) {
		// use this to catch end of recording.
		// std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
		std::cout << "file ended." << std::endl;
	}

	pipe.stop();
	file.seekp(0);
	file.write((char*)&nFrames,sizeof(int32_t));
	file.close();
	file.clear();
	std::cout << nFrames << " frames." << std::endl;
	std::cout << "saved." << std::endl;

}