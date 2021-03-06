#pragma once

#define XRES (1920/2)
#define YRES (1080/2)

// Set this for development of calibration based on reference pictures
#define USE_CALIBRATION_PICTURES

// #define LIST_DEVICES
#if 0  // Microsoft camera
// <--- Set to 0 to use default system webcam.
#define WEBCAM_DEVICE_INDEX 0
//#define MEDIA_TYPE_INDEX 29  /* 320x240, YUY2, 30 FPS, ~37MBPS */
#define MEDIA_TYPE_INDEX 0  /* 640x480, YUY2, 30 FPS, ~147MBPS */
//#define MEDIA_TYPE_INDEX 53  /* 1280x720, YUY2, 30 FPS, ~442MBPS */

#else  // Aukey webcam
// <--- Set to 0 to use default system webcam.
#define WEBCAM_DEVICE_INDEX 0
// Use this for "AUKEY PC-LM1A Webcam"
//#define MEDIA_TYPE_INDEX 62 /* 640x480, YUY2, 30 FPS, ~147MBPS */
#define MEDIA_TYPE_INDEX 12 /* 1920x1080, NV12, 30 FPS */
#define MEDIA_SUBTYPE_NV12  // I should read this from the structure
#endif

// Using multisample sounds like a bad idea
//#define USE_MULTISAMPLE

// Time until calibration starts after program launch in ms
//#define CALIBRATION_DELAY (20 * 1000)
#define CALIBRATION_DELAY (1 * 1000)
// Must be more than in latency measurement
//#define USE_LATENCY 2000
#define USE_LATENCY 200
#define NUM_ACCUMULATE 20

// Resolution of the calibration
#define CALIBRATION_LOG_X_RESOLUTION 8     // width 256
#define CALIBRATION_X_RESOLUTION (1 << CALIBRATION_LOG_X_RESOLUTION)
#define CALIBRATION_LOG_Y_RESOLUTION 7     // height 128
#define CALIBRATION_Y_RESOLUTION (1 << CALIBRATION_LOG_Y_RESOLUTION)

#define CALIBRATION_THRESHOLD 500  // How bright the image has to be to be considered white.
