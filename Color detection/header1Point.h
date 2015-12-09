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

// callibrage
extern double alpha; // [degrees] angle between the laser and the camera axis
extern double D; // [m] distance with which the point of the laser is in the middle of the capture
extern double a0; // [m] distance with which the point of the laser is on a extrem side of the capture
extern int A0; // [pixels] number of pixel corresponding to a0 : image->weight / 2

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
void centroiding(IplImage *image, int xy[], int canal);
double findDistance(IplImage *image, int xy[]);
double findDistance2(IplImage *image, int xy[]);


#endif