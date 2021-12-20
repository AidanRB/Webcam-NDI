#include <cstdlib>
#include <cstring>
#include <chrono>
#include <getopt.h>
#include <iostream>
#include <stdlib.h>

#include <opencv2/opencv.hpp>
#include <Processing.NDI.Lib.h>

#ifdef _WIN32
#ifdef _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x64.lib")
#else // _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x86.lib")
#endif // _WIN64
#endif // _WIN32

int main(int argc, char **argv)
{
	int X = 640;
	int Y = 480;
	int FPS_N = 30;
	int FPS_D = 1;
	int CAM = 0;
	char *NAME = "camera";

	// parsing arguments
	int window_flag = 0;
	int verbose_flag = 1;
	int c;
	while(true) {
		static struct option long_options[] = {
			/* These options set a flag. */
			{"window", no_argument, &window_flag, 1},
			{"no-window", no_argument, &window_flag, 0},
			{"verbose", no_argument, &verbose_flag, 1},
			{"quiet", no_argument, &verbose_flag, 0},
			/* These options don’t set a flag.
			We distinguish them by their indices. */
			{"width", required_argument, 0, 'x'},
			{"height", required_argument, 0, 'y'},
			{"fps", required_argument, 0, 'f'},
			{"fps-d", required_argument, 0, 'd'},
			{"name", required_argument, 0, 'n'},
			{0, 0, 0, 0}
		};

		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "x:y:f:d:n:",
						long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
		case 0:
			/* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0)
				break;
			printf("option %s", long_options[option_index].name);
			if (optarg)
				printf(" with arg %s", optarg);
			printf("\n");
			break;

		case 'x':
			X = atoi(optarg);
			break;

		case 'y':
			Y = atoi(optarg);
			break;

		case 'f':
			FPS_N = atoi(optarg);
			break;

		case 'd':
			FPS_D = atoi(optarg);
			break;

		case 'c':
			CAM = atoi(optarg);
			break;

		case 'n':
			NAME = optarg;
			break;

		default:
			abort();
		}
	}

	// Not required, but "correct" (see the SDK documentation).
	if (!NDIlib_initialize())
		return -1;

	// Create an NDI source that is called "My Video" and is clocked to the video.
	NDIlib_send_create_t NDI_send_create_desc;
	NDI_send_create_desc.p_ndi_name = NAME;

	// create the NDI sender
	NDIlib_send_instance_t pNDI_send = NDIlib_send_create(&NDI_send_create_desc);
	if (!pNDI_send)
		return -1;

	// NDI frame initialization
	NDIlib_video_frame_v2_t NDI_video_frame;
	// resolution
	NDI_video_frame.xres = X;
	NDI_video_frame.yres = Y;
	// type
	NDI_video_frame.FourCC = NDIlib_FourCC_type_BGRA;
	// allocate memory for the actual data
	NDI_video_frame.p_data = (uint8_t *)malloc(X * Y * 4);
	// framerate (N/D)/s
	NDI_video_frame.frame_rate_N = FPS_N;
	NDI_video_frame.frame_rate_D = FPS_D;

	// OpenCV initialization
	cv::Mat frame;
	cv::Mat NDIframe;
	cv::VideoCapture cap;
	// resolution of capture
	cap.set(cv::CAP_PROP_FRAME_WIDTH, X);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, Y);
	// open the default camera using default API
	cap.open(CAM);
	// check if we succeeded or if the camera is cap
	if (!cap.isOpened())
	{
		std::cerr << "ERROR! Unable to open camera\n";
		return -1;
	}

	using namespace std::chrono;
	const auto start = high_resolution_clock::now();
	while(true) {
		// Get the current time
		const auto start_send = high_resolution_clock::now();

		// Send 60 frames
		for (int idx = 60; idx; idx--)
		{
			// gets image from capture in frame
			cap.read(frame);
			// change color from BGR to BGRA, NDI has transparency apparently
			cv::cvtColor(frame, NDIframe, cv::COLOR_BGR2BGRA);

			// set p_data to point to the data in NDIframe
			NDI_video_frame.p_data = (uint8_t *)NDIframe.data;

			// send the frame
			NDIlib_send_send_video_v2(pNDI_send, &NDI_video_frame);

			// show the opencv frame captured from the camera
			if (window_flag) {
				cv::imshow("output", frame);
				cv::waitKey(1);
			}
		}

		// output the FPS
		if(verbose_flag) {
			std::cout << "60 frames sent, at " << 60.0 / duration_cast<duration<float>>(high_resolution_clock::now() - start_send).count() << "fps" << std::endl;
		}
	}

	// Free the video frame
	free(NDI_video_frame.p_data);

	// Destroy the NDI sender
	NDIlib_send_destroy(pNDI_send);

	// Not required, but nice
	NDIlib_destroy();

	// Success
	return 0;
}
