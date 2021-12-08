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
	// NDI_video_frame.p_data = (uint8_t*)malloc(X * Y * 4);
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
    // check if we succeeded
    if (!cap.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }


	// Run for one minute
	using namespace std::chrono;
	for (const auto start = high_resolution_clock::now(); high_resolution_clock::now() - start < minutes(5);)
	{	// Get the current time
		const auto start_send = high_resolution_clock::now();

		// Send 200 frames
		for (int idx = 200; idx; idx--)
		{	// Fill in the buffer. It is likely that you would do something much smarter than this.
			// memset((void*)NDI_video_frame.p_data, (idx & 1) ? 255 : 0, NDI_video_frame.xres*NDI_video_frame.yres * 4);

			// something much smarter than that^
			cap.read(frame);
			NDI_video_frame.p_data = (uint8_t*)frame.data;
			// send the frame
			NDIlib_send_send_video_v2(pNDI_send, &NDI_video_frame);

			// show the opencv fram captured from the camera
			cv::imshow("output", frame);
			cv::waitKey(1);
		}

		// Just display something helpful
		std::cout << "200 frames sent, at " << 200.0 / duration_cast<duration<float>>(high_resolution_clock::now() - start_send).count() << "fps" << std::endl;
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

