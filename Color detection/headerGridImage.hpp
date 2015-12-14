//
//  headerGrille.hpp
//  Exos
//
//  Created by Katleen Blanchet on 06/12/15.
//
//

#ifndef headerGrille_hpp
#define headerGrille_hpp

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//#include <opencv\cv.hpp>
#include <cv.hpp>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctime>


#include<iostream>
#include<cmath>
#include <vector>
using std::vector;
using namespace std;

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
vector<CvPoint3D32f> pixelsDetectedCoordinates(IplImage *image, int canal);
vector<vector<CvPoint3D32f> > pointsDistinction(vector<CvPoint3D32f> vec);
bool searchNeighboursPixel(vector<CvPoint3D32f> vec, CvPoint3D32f pix);
bool searchNeighboursVect(vector<CvPoint3D32f> vecPoint1, vector<CvPoint3D32f> vecPoint2);
vector<CvPoint> centroiding(vector<vector<CvPoint3D32f> > points);
vector<CvPoint> sort(vector<CvPoint> tab);
void insert(vector<CvPoint> &tab, CvPoint point, int i);
/*double findDistance(IplImage *image, int xy[]);*/
vector<double> calibrate();
vector<double> findDistance2(IplImage *image, vector<CvPoint> centroid, vector<double> tanAlphaT);
void findPointRec(vector<CvPoint3D32f> &point, CvPoint pixel, uchar* data, int step);
vector<vector<CvPoint3D32f> > findPoint();

#endif /* headerGrille_hpp */
