#ifndef HEADER_H_
#define HEADER_H_

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <cv.hpp>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>


extern int seuilFiltre;

extern int hmin; extern int hmax;
extern int smin; extern int smax;
extern int vmin; extern int vmax;

extern int nbErosions;
extern int nbDilatations;

extern IplImage *frame;
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



#endif