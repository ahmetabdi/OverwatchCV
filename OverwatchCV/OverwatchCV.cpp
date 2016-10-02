// OverwatchCV.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;
using namespace cv;

Mat hwnd2mat(HWND hwnd) {

	HDC hwindowDC, hwindowCompatibleDC;

	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	Mat src;
	BITMAPINFOHEADER  bi;

	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwnd, &windowsize);

	srcheight = windowsize.bottom / 2;
	srcwidth = windowsize.right / 2;
	height = windowsize.bottom / 4;  //change this to whatever size you want to resize to
	width = windowsize.right / 4;
	int startX = windowsize.right / 4;
	int startY = windowsize.bottom / 4;

	src.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, startX, startY, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

	DeleteObject(hbwindow); DeleteDC(hwindowCompatibleDC); ReleaseDC(hwnd, hwindowDC);

	return src;
}

int main()
{
	/*
	Mat img = hwnd2mat(GetDesktopWindow());
	if (img.empty())
	{
		cout << "Error : Image cannot be loaded....." << endl;
		return -1;
	}
	*/

	namedWindow("Control", CV_WINDOW_AUTOSIZE);

	for(;;) {
		double t = (double)getTickCount();
		Mat img = hwnd2mat(GetDesktopWindow());
		Mat imgHSV;
		Mat1b mask1, mask2;

		cvtColor(img, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

		inRange(imgHSV, Scalar(0, 70, 50), Scalar(10, 255, 255), mask1);
		inRange(imgHSV, Scalar(170, 70, 50), Scalar(180, 255, 255), mask2);
		Mat1b mask = mask1 | mask2;


		//Mat bwImage;
		//Mat convertedImage;
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;

		//cvtColor(img, bwImage, CV_RGB2GRAY);
		//bwImage.convertTo(convertedImage, CV_8UC1);
		findContours(mask, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

		//cout << "Found: " << contours.size() << endl;

		vector<double> contourAreas;
		for (int i = 0; i < contours.size(); i++) {
			double area = contourArea(contours[i]);
			contourAreas.push_back(area);


			Point2f center;
			float radius;

			cout << "Found: " << contours[i].size() << endl;
			minEnclosingCircle(contours[i], center, radius);
			circle(img, center, radius, Scalar(0, 0, 255), 2);
		}

		//double maxArea = *max_element(contourAreas.begin(), contourAreas.end());
		double maxAreaIndex = distance(contourAreas.begin(), max_element(contourAreas.begin(), contourAreas.end()));

		Point2f center;
		float radius;

		

		minEnclosingCircle(contours[maxAreaIndex], center, radius);
		circle(img, center, radius, Scalar(0, 0, 255), 2);

		cout << "Center: " << center << " Radius: " << radius << endl;

		imshow("Thresholded Image", mask); //show the thresholded image
		imshow("Original", img); //show the original image
		//imshow("Original", imgHSV); //show the original image
		/*
		imshow("MYWINDOW", img);
		
		*/

		// The following code computes the execution time in seconds and outputs to consoled should be around 0.01
		t = ((double)getTickCount() - t) / getTickFrequency();

		int time_in_seconds = t / 1000;

		//cout << t << endl;
		
		// Update every 1 millisecond :)
		waitKey(1);
	}
	

	waitKey(0);

	cvvDestroyWindow("MYWINDOW");

    return 0;
}