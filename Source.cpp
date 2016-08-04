#include <iostream>
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
int main() {
	
	VideoCapture capture(0);
	if (!capture.isOpened()) {
		std::cout << "Cannot open webcam" << std::endl;
		return -1;
	}

	Trackerbar();

	namedWindow("Video", WINDOW_AUTOSIZE);
	
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

		imshow("Video", thresholdedImg);

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