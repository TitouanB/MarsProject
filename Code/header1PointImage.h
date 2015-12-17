#ifndef HEADER_H_
#define HEADER_H_

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//#include <opencv\cv.hpp>
#include <cv.hpp>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include<iostream>
#include<cmath>
#include <vector>
#include <ctime>
using std::vector;
using namespace std;

// calibration
extern double D; // [m] distance with which the point of the laser is in the middle of the capture
extern double d; // [m] distance between the laser and the camera
extern double tanAlpha; // [rad] tangent of the angle between the laser and the camera axis
extern double fx; // [pixels] focal length in the horizontal direction
extern double fy; // [pixels] focal length in the vertical direction
extern double principalPointx; // [pixels] optical center x direction
extern double principalPointy; // [pixels] optical center y direction
extern double k1; // radial distortion coefficient of the lens
extern double k2; // radial distortion coefficient of the lens
extern double p1; // tangential distortion coefficient of the lens
extern double p2; // tangential distortion coefficient of the lens

extern int seuilFiltre;

extern int hmin; extern int hmax;
extern int smin; extern int smax;
extern int vmin; extern int vmax;

extern int nbErosions;
extern int nbDilatations;

extern IplImage* frame;
extern IplImage* imageFiltree;
extern IplImage* imageHSV;
extern IplImage* imageBinaire;
extern IplImage* imageErodee;
extern IplImage* imageDilatee;
extern IplImage* imageFinale;

extern const char *myWindow;

extern CvMemStorage* storage;
extern CvSeq* contours;

void callback(int i);
IplImage* multiplier(IplImage *image1, IplImage *image2);
IplImage* multBinColor(IplImage *imageBin, IplImage *imageColor);
IplImage* lowPassFilter(IplImage *image);
void findPointRec(vector<CvPoint3D32f> &point, CvPoint pixel, uchar* data, int step);
vector<vector<CvPoint3D32f> > findPoint();
vector<CvPoint> centroiding(vector<vector<CvPoint3D32f> > points);
double findDistance(IplImage *image, CvPoint centroid);
double findDistanceLensDistortion(IplImage *image, CvPoint centroid);


#endif