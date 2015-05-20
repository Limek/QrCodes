#ifndef decode_qr_code_h
#define decode_qr_code_h

#include <stdio.h>
#include <iostream>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include "opencv/cv.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"
#include <vector>
#include "zbar.h"

using namespace std;
using namespace cv;
using namespace zbar;

class code_qr
{
	private:
	Mat img;
	public:
	Mat pic;
	double qrAngle;
	double qrDistance;
	vector<vector<Point> > contours;
	vector<vector<Point> > squares_contours;
	vector<Point> theChosenOne;
	void add_contour(vector<Point>& contour); // dodaje kontur do wektora contours
	void cut_qr(Mat& img_all);
	void fun(int a);
	static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0); // funkcja z neta liczaca jakis katy
	void decoder(Mat& img_qr, string& message); // funckja czytajaca kod QR
	void detect_squares(); // funkcja szukajaca kwadratow wsrod konturow
	void detect_the_chosen_one();



};

#endif
