#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// define the necessary matrices for the program
Mat img, imgHSV, mask, canny_output;

// Hue, saturation, and value ranges that for the cone color
int hmin = 0, smin = 201, vmin = 142;
int hmax = 179, smax = 255, vmax = 255;

void main() {
	// saves originial image into matrix
	string path = "redcone.png";
	img = imread(path);
	
	// Converting image into hsv color space for better detection
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	// Generating image mask to isolate cone colored pixels
	Scalar lower(hmin, smin, vmin);
	Scalar upper(hmax, smax, vmax);
	inRange(imgHSV, lower, upper, mask);

	vector<vector<Point> > contours; // vector with vectors of type Point
	vector<Vec4i> hierarchy; // Vector with 4 int data type

	// finds edges of cones
	Canny(mask, canny_output, 50, 150, 3);
	
	// finds the contours of all the cones
	findContours(canny_output, contours, hierarchy, RETR_TREE,
		CHAIN_APPROX_SIMPLE, Point(0, 0));

	// gets the image moments of cone pixel blobs and appends them to a vector
	vector<Moments> mu(contours.size());
	for (int i = 0; i < contours.size(); i++) {
		mu[i] = moments(contours[i], false);
	}

	// gets the centroid of all cones and appends them to a vector
	vector<Point2f> mc(contours.size());
	for (int i = 0; i < contours.size(); i++) {
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}


	vector<Point2f> leftSide;
	vector<Point2f> rightSide;
	for (int i = 0; i < contours.size(); i++)
	{
		// removes any duplicate centroids of cone blobs
		if (i == contours.size() - 1) {
			if (mc[i].x - mc[i - 1].x < 1 && mc[i].x - mc[i - 1].x > -1) {
				break;
			}
		} else if (mc[i].x - mc[i + 1].x < 1 && mc[i].x - mc[i + 1].x > -1){
			continue;
		}
		int area = contourArea(contours[i]);
		// isolates cone centroids from other artifacts and checks to see which 
		// cones are on the left line and which are on the right
		if (area > 8) {
			if (mc[i].x < 908) {
				leftSide.push_back(mc[i]);
			} else if (mc[i].x > 908) {
				rightSide.push_back(mc[i]);
			}
		}
	}

	// finds beginning point that is closest to bottom of the image
	Point2f leftLow = leftSide[0];
	Point2f rightLow = rightSide[0];
	for (int i = 1; i < leftSide.size(); i++) {
		if (leftSide[i].y > leftLow.y) {
			leftLow = leftSide[i];
		}
		if (rightSide[i].y > rightLow.y) {
			rightLow = rightSide[i];
		}
	}

	// finds end point that is closest to top of the image
	Point2f leftHigh = leftSide[0];
	Point2f rightHigh = rightSide[0];
	for (int i = 1; i < leftSide.size(); i++) {
		if (leftSide[i].y < leftHigh.y) {
			leftHigh = leftSide[i];
		}
		if (rightSide[i].y < rightHigh.y) {
			rightHigh = rightSide[i];
		}
	}

	// finds horizontal and vertical shift (slope) of both points
	float leftWidthDir = leftHigh.x - leftLow.x;
	float leftHeightDir = leftLow.y - leftHigh.y;
	float rightWidthDir = rightLow.x - rightHigh.x;
	float rightHeightDir = rightLow.y - rightHigh.y;

	float scale = 3; // line scaling factor

	// scales line to extend beyond the cones
	leftLow = Point2f(leftLow.x - (leftWidthDir * scale), leftLow.y + (leftHeightDir * scale));
	leftHigh = Point2f(leftHigh.x + (leftWidthDir * scale), leftHigh.y - (leftHeightDir * scale));
	rightLow = Point2f(rightLow.x + (rightWidthDir * scale), rightLow.y + (rightHeightDir * scale));
	rightHigh = Point2f(rightHigh.x - (rightWidthDir * scale), rightHigh.y - (rightHeightDir * scale));

	// draw lines
	line(img, leftLow, leftHigh, Scalar(0, 0, 255), 3);
	line(img, rightLow, rightHigh, Scalar(0, 0, 255), 3);

	// show image and save to file
	imshow("Image", img);
	imwrite("answer.png", img);
	waitKey(0);
}