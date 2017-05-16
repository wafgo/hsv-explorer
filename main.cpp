#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include <iostream>
#include "decoder/mjpeg-decoder-sw.h"
#include "include/stream/v4l2-stream-device.h"
#include <iomanip>

using namespace cv;
using namespace std;

#define VIDEO_WIDTH 1280
#define VIDEO_HEIGHT 720

#define TEXT_Y_OFFSET 6

enum mouse_window_cap_state {
		MW_WAIT_FOR_START_POINT, MW_WAIT_FOR_END_POINT
};

struct mouse_window {
	Point rectP1, rectP2;
};

static struct mouse_window mw;
enum mouse_window_cap_state mw_state = MW_WAIT_FOR_START_POINT;

static void mouse_callback(int event, int x, int y, int flags, void* userdata)
{
	switch (mw_state)
	{
	case MW_WAIT_FOR_START_POINT:
		if (event == EVENT_LBUTTONDOWN) {
			mw.rectP1.x = x;
			mw.rectP1.y = y;
			mw_state = MW_WAIT_FOR_END_POINT;
		}
		break;
	case MW_WAIT_FOR_END_POINT:
		mw.rectP2.x = x;
		mw.rectP2.y = y;
		if (event == EVENT_LBUTTONUP) {
			mw_state = MW_WAIT_FOR_START_POINT;
		}
		break;
	}
}

static Size set_label(cv::Mat& im, const std::string label, const cv::Point & origin)
{
    int fontface = FONT_HERSHEY_TRIPLEX;
    double scale = 0.5;
    int thickness = 1;
    int baseline = 0;

    Size text = getTextSize(label, fontface, scale, thickness, &baseline);
    rectangle(im, origin + Point(0, baseline), origin + Point(text.width, -text.height), CV_RGB(0,0,0), CV_FILLED);
    putText(im, label, origin, fontface, scale, CV_RGB(255,255,255), thickness, 8);
    return text;
}

int main(int argc, char** argv)
{
	cv::CommandLineParser parser(argc, argv,
			"{help h|| print this help}"
			"{camera | /dev/video0 | camera device}"
			"{video-width | 1280 | desired width of the video frames}"
			"{video-height | 720 | desired height of the video frames}"

	);

	if (parser.has("help")) {
		parser.printMessage();
		exit(-1);
	}
	VideoStreamDevice* videoDev = new V4LStreamDevice(parser.get<string>("camera"), parser.get<int>("video-width"), parser.get<int>("video-height"));
	DecoderDevice* mjpegDecoder = new MJPEGDecoderDevice;
	char* rgb = new char[videoDev->getWidth() * videoDev->getHeight() * 3];
	Mat img = Mat(videoDev->getHeight(), videoDev->getWidth(), CV_8UC3, rgb);
	Mat hsv;
	Mat roi;
	Scalar _mean;
	int txt_height;
	ostringstream ht, st, vt;
	namedWindow("depth", 1);
	setMouseCallback("depth", mouse_callback, NULL);

	while (1) {
		struct videoStreamBuffer vbuf;
		videoDev->grabOneFrame();
		videoDev->getBuffer(&vbuf);
		Rect selected_window(mw.rectP1, mw.rectP2);
		mjpegDecoder->decode(vbuf.data, vbuf.len, videoDev->getWidth(),
				videoDev->getHeight(), rgb);
		cvtColor(img, img, COLOR_RGB2BGR);
		cvtColor(img, hsv, COLOR_BGR2HSV);

		rectangle(img, mw.rectP1, mw.rectP2, Scalar(255, 255, 255),
					1, LINE_8);
		if (selected_window.area() > 0) {
			Size sz;
			Point anchor = mw.rectP1;
			ht.str("");
			st.str("");
			vt.str("");
			roi = hsv(selected_window);
			_mean = mean(roi);
			ht << std::fixed << setprecision(0) << "h: " << _mean[0];
			anchor.y -= TEXT_Y_OFFSET;
			sz = set_label(img, ht.str(), anchor);
			st << std::fixed << setprecision(0) << "s: "<< _mean[1];
			anchor.y -= (sz.height + TEXT_Y_OFFSET);
			sz = set_label(img, st.str(), anchor);
			vt << std::fixed << setprecision(0) << "v: "<< _mean[2];
			anchor.y -= (sz.height + TEXT_Y_OFFSET);
			sz = set_label(img, vt.str(), anchor);
		}
		imshow("depth", img);
		waitKey(10);
	}

	return 0;
}
