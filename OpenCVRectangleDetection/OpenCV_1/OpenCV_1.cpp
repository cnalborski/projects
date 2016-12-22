#include "stdafx.h"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace cv;

string window_name = "Capture - live";
string window2_name = "Capture - processed";

int thresh = 150;
int max_thresh = 255;

void delay(unsigned int mseconds)
{
	clock_t goal = mseconds + clock();
	while (goal > clock());
}

static double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

void detectAndDisplay(Mat frame)
{
	Mat src_gray;
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	cvtColor(frame, src_gray, CV_BGR2GRAY);
	//blur(src_gray, src_gray, Size(3, 3));

	/// Detect edges using canny
	Canny(src_gray, canny_output, thresh, thresh * 2, 3);
	/// Find contours
	findContours(canny_output, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Draw contours
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	vector<Point> approx;
	for (int i = 0; i< contours.size(); i++)
	{
		approxPolyDP(contours[i], approx, arcLength(contours[i], true)*0.02, true);
		if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
			continue;

		if (approx.size() == 4)
		{
			int vtc = approx.size();

			// Get the cosines of all corners
			std::vector<double> cos;
			for (int j = 2; j < vtc + 1; j++)
				cos.push_back(angle(approx[j%vtc], approx[j - 2], approx[j - 1]));

			// Sort ascending the cosine values
			sort(cos.begin(), cos.end());

			// Get the lowest and the highest cosine
			double mincos = cos.front();
			double maxcos = cos.back();

			// Use the degrees obtained above and the number of vertices
			// to determine the shape of the contour
			if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3) {
				putText(frame, "Rectangle detected!", Point(10, 30), FONT_HERSHEY_PLAIN, 2, Scalar(255, 255, 0), 2);

				Point textPos;
				textPos.x = approx.at(0).x + approx.at(1).x + approx.at(2).x + approx.at(3).x;
				textPos.y = approx.at(0).y + approx.at(1).y + approx.at(2).y + approx.at(3).y;
				textPos.x /= 4.0;
				textPos.y /= 4.0;
				textPos.x -= 20;
				putText(frame, "RECTANGLE", textPos, FONT_HERSHEY_PLAIN, .5, Scalar(255, 255, 255), 1);

				line(frame, approx.at(0), approx.at(1), Scalar(255, 0, 255), 4);
				line(frame, approx.at(1), approx.at(2), Scalar(255, 0, 255), 4);
				line(frame, approx.at(2), approx.at(3), Scalar(255, 0, 255), 4);
				line(frame, approx.at(3), approx.at(0), Scalar(255, 0, 255), 4);
				//drawContours(frame, contours, i, Scalar(100, 100, 10), CV_FILLED, 8, hierarchy);
			}
		}
	}

	//-- Show what you got
	imshow(window_name, frame);
	imshow(window2_name, canny_output);
}

int main(int argc, const char** argv)
{
	CvCapture* capture;
	Mat frame;

	capture = cvCaptureFromCAM(0);
	printf("Starting camera...\n");
	delay(1000 * 3);
	if (capture)
	{
		while (true)
		{
			//frame = cvQueryFrame(capture);
			frame = cvQueryFrame(capture);

			if (!frame.empty())
			{
				detectAndDisplay(frame);
			}
			else
			{
				printf(" --(!) No captured frame -- Break!"); break;
			}

			int c = waitKey(10);
			if ((char)c == 'c') { break; }
		}
	}
	else {
		printf("No capture!\n");
	}

	return 0;
}


