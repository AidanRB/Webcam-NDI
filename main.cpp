#include <iostream>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <Processing.NDI.Lib.h>
#include <opencv2/opencv.hpp>

#ifdef _WIN32
#ifdef _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x64.lib")
#else // _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x86.lib")
#endif // _WIN64
#endif // _WIN32

int main(int argc, char* argv[])
{	
	const int X = 640;
	const int Y = 480;
	
	// Not required, but "correct" (see the SDK documentation).
	if (!NDIlib_initialize()) return 0;
	
	// create the NDI sender
	NDIlib_send_instance_t pNDI_send = NDIlib_send_create();
	if (!pNDI_send) return 0;


	// NDI frame initialization
	NDIlib_video_frame_v2_t NDI_video_frame;
	// resolution
	NDI_video_frame.xres = X;
	NDI_video_frame.yres = Y;
	// type
	NDI_video_frame.FourCC = NDIlib_FourCC_type_BGRA;
	// allocate memory for the actual data
	NDI_video_frame.p_data = (uint8_t*)malloc(X * Y * 4);
	// framerate (N/D)/s
	NDI_video_frame.frame_rate_N = 60;
	NDI_video_frame.frame_rate_D = 1;


	// OpenCV initialization
	cv::Mat frame;
	cv::Mat NDIframe;
    cv::VideoCapture cap;
	// resolution of capture
	cap.set(cv::CAP_PROP_FRAME_WIDTH, X);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, Y);
    // open the default camera using default API
    cap.open(0);
    // check if we succeeded or if the camera is cap
    if (!cap.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }


	// Run for five minutes
	using namespace std::chrono;
	const auto start = high_resolution_clock::now();
	while (high_resolution_clock::now() - start < minutes(5)) {

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
			NDI_video_frame.p_data = (uint8_t*)NDIframe.data;

			// send the frame
			NDIlib_send_send_video_v2(pNDI_send, &NDI_video_frame);

			// show the opencv frame captured from the camera
			//cv::imshow("output", frame);
			//cv::waitKey(1);
		}

		// output the FPS
		std::cout << "60 frames sent, at " << 60.0 / duration_cast<duration<float>>(high_resolution_clock::now() - start_send).count() << "fps" << std::endl;
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

