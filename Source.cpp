#include <iostream>
#include <sstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;

Mat frame;
Mat HSVFrame;
Mat thresholdedImg;
int lowerH = 0, lowerS = 0, lowerV = 0, upperH = 179, upperS = 255, upperV = 255;
/*
Hue values of basic colors
	Orange  0-22
	Yellow 22- 38
	Green 38-75
	Blue 75-130
	Violet 130-160
	Red 160-179
*/
void Trackerbar();
void CallbackForLowerH(int val, void * userData);
void CallbackForLowerS(int val, void * userData);
void CallbackForLowerV(int val, void * userData);
void CallbackForUpperH(int val, void * userData);
void CallbackForUpperS(int val, void * userData);
void CallbackForUpperV(int val, void * userData);

void openingOperation();
void closingOperation();

void trackObj(const Mat thresholdedImg, Mat &frame);
void tagObjectPosition(Mat &frame, int x, int y);

int main() {
	
	VideoCapture capture(0);
	if (!capture.isOpened()) {
		std::cout << "Cannot open webcam" << std::endl;
		return -1;
	}

	Trackerbar();

	namedWindow("Video", WINDOW_AUTOSIZE);
	namedWindow("Threshold", WINDOW_AUTOSIZE);
	while (1) {
		bool succeed = capture.read(frame);
		if (!succeed) {
			std::cout << "Cannot read image from webcam" << std::endl;
			break;
		}
		cvtColor(frame, HSVFrame, CV_BGR2HSV);

		
		inRange(HSVFrame,Scalar(lowerH,lowerS,lowerV), Scalar(upperH, upperS, upperV),thresholdedImg );

		//openingOperation();
		closingOperation();

		trackObj(thresholdedImg, frame);

		imshow("Threshold", thresholdedImg);
		imshow("Video", frame);

		if (waitKey(30) == 27) {
			break;
		}
	}
}

void Trackerbar() {
	namedWindow("Control", WINDOW_AUTOSIZE);
	createTrackbar("LowerH","Control", &lowerH, 179,CallbackForLowerH, NULL );
	createTrackbar("UpperH", "Control", &upperH, 179, CallbackForUpperH, NULL);
	createTrackbar("LowerS", "Control", &lowerS, 255, CallbackForLowerS, NULL);
	createTrackbar("UpperS", "Control", &upperS, 255, CallbackForUpperS, NULL);
	createTrackbar("LowerV", "Control", &lowerV, 255, CallbackForLowerV, NULL);
	createTrackbar("UpperV", "Control", &upperV, 255, CallbackForUpperV, NULL);
}

void CallbackForLowerH(int val, void * userData) {
	lowerH = val;
}
void CallbackForLowerS(int val, void * userData) {
	lowerS = val;
}
void CallbackForLowerV(int val, void * userData) {
	lowerV = val;
}
void CallbackForUpperH(int val, void * userData) {
	upperH = val;
}
void CallbackForUpperS(int val, void * userData) {
	upperS = val;
}
void CallbackForUpperV(int val, void * userData) {
	upperV = val;
}

void openingOperation() {
	//dilate followed by erode
	Mat erodeMat = getStructuringElement(MORPH_RECT, Size(3,3));
	Mat dilateMat = getStructuringElement(MORPH_RECT, Size(8, 8));
	
	erode(thresholdedImg, thresholdedImg, erodeMat);
	erode(thresholdedImg, thresholdedImg, erodeMat);

	dilate(thresholdedImg,thresholdedImg, dilateMat);
	dilate(thresholdedImg, thresholdedImg, dilateMat);

}
void closingOperation() {
	//erode followed by dilate
	Mat erodeMat = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat dilateMat = getStructuringElement(MORPH_RECT, Size(8, 8));

	dilate(thresholdedImg, thresholdedImg, dilateMat);
	dilate(thresholdedImg, thresholdedImg, dilateMat);

	erode(thresholdedImg, thresholdedImg, erodeMat);
	erode(thresholdedImg, thresholdedImg, erodeMat);
}

void trackObj(const Mat thresholdedImg, Mat &frame) {
	
	Mat temp;
	thresholdedImg.copyTo(temp);

	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;	//[Next, Previous, First_Child, Parent]	
	
	findContours(temp, contours, hierarchy,CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	/*for more detail about findContours, see the link below
	http://docs.opencv.org/3.1.0/d9/d8b/tutorial_py_contours_hierarchy.html#gsc.tab=0
	*/
	int x, y;
	double area = 0;
	bool objFound = false;

	if (hierarchy.size() > 0) {
		if (hierarchy.size() < 5) {	//If hierarchy.size() is over 5, there may be too many noise.
			for (int i = 0; i >= 0; i = hierarchy[i][0]) {
				Moments moment = moments(contours[i]);
				double area = moment.m00;

				if (area > 50*50 && area < 300*300) {
					x = moment.m10 / area;
					y = moment.m01 / area;
					objFound = true;
				}
				else {
					objFound = false;
				}
				if (objFound) {
					tagObjectPosition(frame, x, y);
				}
			}
		}
		else 
			putText(frame, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}

void tagObjectPosition(Mat &frame, int x, int y) {
	circle(frame, cv::Point(x, y), 10, cv::Scalar(0, 0, 255));
	
	std::ostringstream string;
	string << x << " , " << y;
	
	putText(frame, string.str(), cv::Point(x, y + 20), 1, 1, Scalar(0, 255, 0));
}