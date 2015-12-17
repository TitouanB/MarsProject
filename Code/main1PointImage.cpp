#include "header1PointImage.h"

IplImage* frame = 0;
IplImage* imageFiltree;
IplImage* imageHSV;
IplImage* imageBinaire;
IplImage* imageErodee;
IplImage* imageDilatee;
IplImage* imageDilateeFiltree;
IplImage* imageObjectHSV;
IplImage* imageObjectRGB;
IplImage* imageFinale;

int seuilFiltre = 1;

const char *myWindow = "Flux Webcam";
const char *myWindowObjectHSV = "Detected Object HSV";
const char *myWindowObjectRGB = "Detected Object RGB";

int hmin = 35; int hmax = 100; // green beam
int smin = 0; int smax = 135;
int vmin = 210; int vmax = 255;
/*int hmin = 85; int hmax = 105;// blue box
int smin = 115; int smax = 255;
int vmin = 135; int vmax = 255;*/

int nbErosions = 0;
int nbDilatations = 0;

CvMemStorage* storage;
CvSeq* contours=0;

// calibration
#define PI 3.14159265358979323846
double D = 1.37;
double d = 0.3;
double tanAlpha = d/D;
double fx = 825.566622568997;
double fy = 805.626003246457;
double principalPointx = 378.441275109819;
double principalPointy = 175.276601699163;
double k1 = 0.132411320207640;
double k2 = -0.176208274323161;
double p1 = -0.0205931237124544;
double p2 = 0.0138391674462332;

int main(int argc, char *argv[])
{
	char k;
	
	// CAMERA
	/*CvCapture *capture = cvCreateCameraCapture(1);
	
	frame = cvQueryFrame(capture);*/
	
	// IMAGE
	const char *imageFile = "./98dot5cm.jpg";
	
	frame = cvLoadImage(imageFile,CV_LOAD_IMAGE_COLOR);
	
	// initialization
	imageFiltree = cvCreateImage(cvGetSize(frame),frame->depth,frame->nChannels);
	imageHSV = cvCreateImage(cvGetSize(frame),frame->depth,frame->nChannels);
	imageBinaire = cvCreateImage(cvGetSize(frame),frame->depth,1);
	imageErodee = cvCreateImage(cvGetSize(frame),frame->depth,1);
	imageDilatee = cvCreateImage(cvGetSize(frame),frame->depth,1);
	imageDilateeFiltree = cvCreateImage(cvGetSize(frame),frame->depth,1);
	imageObjectHSV = cvCreateImage(cvGetSize(frame),frame->depth,frame->nChannels);
	imageObjectRGB = cvCreateImage(cvGetSize(frame),frame->depth,frame->nChannels);
	imageFinale = cvCreateImage(cvGetSize(frame),frame->depth,frame->nChannels);
	
	storage = cvCreateMemStorage(0);
	
	printf("Tuning of the Color Detection\n");
	printf("Press q to end the tuning\n\n");
	
	for (;;)
	{
		// IMAGE
		frame = cvLoadImage(imageFile,CV_LOAD_IMAGE_COLOR);
		
		/*// CAM
		 frame = cvQueryFrame(capture);*/
		
		//cvSmooth(frame, imageFiltree, CV_BLUR,seuilFiltre,seuilFiltre,0.0,0.0);
		
		if (!frame)
			break;
		
		callback(0);
		cvCreateTrackbar("seuilFiltre", myWindow,&seuilFiltre, 11, callback);
		cvCreateTrackbar("H min", myWindow, &hmin,180, callback);
		cvCreateTrackbar("H max", myWindow, &hmax,180, callback);
		cvCreateTrackbar("S min", myWindow, &smin,255, callback);
		cvCreateTrackbar("S max", myWindow, &smax,255, callback);
		cvCreateTrackbar("V min", myWindow, &vmin,255, callback);
		cvCreateTrackbar("V max", myWindow, &vmax,255, callback);
		cvCreateTrackbar("nbDilatations", myWindow, &nbDilatations,10, callback);
		cvCreateTrackbar("nbErosions", myWindow, &nbErosions,10, callback);
		
		int delay = 10;
		k=cvWaitKey(delay);
		
		
		if(k=='s'){
			printf("Save\n");
			cvSaveImage("PointContours.jpg", frame,0);
			cvSaveImage("PointHSV.jpg", imageObjectHSV,0);
			
		}
		if (k=='q'){
			printf("Tuning completed\n\n");
			break;
		}
	}
	
	float time;
	clock_t t1, t2;
	
	// Start timer
	t1 = clock();
	
	// find the point
	vector<vector<CvPoint3D32f> > vecDistinctPoints = findPoint();
	
	// find the centroid of the point and trace it
	vector<CvPoint> centroidVector = centroiding(vecDistinctPoints);
	
	// compute the distance with and without lens distortion
	CvPoint centroid = centroidVector[0];
	double distance = findDistance(imageObjectHSV, centroid);
	printf("Distance camera-target without distortion = %f m\n", distance);
	
	double distanceLensDistortion = findDistanceLensDistortion(imageObjectHSV, centroid);
	printf("Distance camera-target with lens distortion = %f m\n\n", distanceLensDistortion);
	
	// End timer
	t2 = clock();
	
	// Compute execution time
	time = (float)(t2 - t1) / CLOCKS_PER_SEC;
	
	cout << "Execution time for the distances = " << time << " s\n" << endl;
	
	cvNamedWindow(myWindowObjectHSV, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(myWindowObjectRGB, CV_WINDOW_AUTOSIZE);
	cvShowImage(myWindowObjectHSV, imageObjectHSV);
	cvShowImage(myWindowObjectRGB, imageObjectRGB);
	
	printf("Press a key to quit\n");
	
	cvWaitKey(0);
	
	cvDestroyWindow(myWindowObjectHSV);
	cvDestroyWindow(myWindowObjectRGB);
	cvDestroyWindow(myWindow);
	//CAM cvReleaseCapture(&capture);
	cvReleaseImage(&imageFiltree);
	cvReleaseImage(&imageBinaire);
	cvReleaseImage(&imageErodee);
	cvReleaseImage(&imageDilatee);
	cvReleaseImage(&imageDilateeFiltree);
	cvReleaseImage(&imageObjectHSV);
	cvReleaseImage(&imageObjectRGB);
	cvReleaseImage(&imageFinale);
	cvReleaseMemStorage(&storage);
}

void callback(int i)
{
	float time;
	clock_t t1, t2;
	
	// Start timer
	t1 = clock();
	
	// Filtering, HSV to Binary Image, Erosions and Dilations
	cvSmooth(frame, imageFiltree, CV_BLUR,seuilFiltre,seuilFiltre,0.0,0.0);
	cvCvtColor(imageFiltree, imageHSV,CV_BGR2HSV);
	cvInRangeS(imageHSV,cvScalar(hmin, smin, vmin, 0.0),cvScalar(hmax, smax, vmax, 0.0),imageBinaire);
	cvErode(imageBinaire, imageErodee, NULL, nbErosions);
	cvDilate(imageErodee, imageDilatee, NULL, nbDilatations);
	
	//imageDilateeFiltree =  lowPassFilter(imageDilatee); FILTER
	
	// multiplication between the original image in RGB and HSV and the binary image
	imageObjectRGB = multBinColor(imageDilatee, frame);
	imageObjectHSV = multBinColor(imageDilatee, imageHSV);
	
	// Contours
	cvFindContours( imageDilatee, storage, &contours, sizeof(CvContour),
				   CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0,0) );
	
	cvDrawContours( frame, contours,
				   CV_RGB(255,255,0), CV_RGB(0,255,0),
				   1, 2, 8, cvPoint(0,0));
	
	
	cvNamedWindow(myWindow, CV_WINDOW_AUTOSIZE);
	cvShowImage(myWindow, frame);
	
	// End timer
	t2 = clock();
	
	// Compute execution time
	time = (float)(t2 - t1) / CLOCKS_PER_SEC;
	
	cout << "execution time callback = " << time << " s" << endl;
	
}

IplImage* multiplier(IplImage *image1, IplImage *image2){
	
	IplImage *res = cvCreateImage(cvGetSize(image1),image1->depth,image1->nChannels);
	
	IplImage *imgCanal1 = cvCreateImage(cvGetSize(image1),image1->depth,1);
	IplImage *imgCanal2 = cvCreateImage(cvGetSize(image1),image1->depth,1);
	IplImage *imgCanal3 = cvCreateImage(cvGetSize(image1),image1->depth,1);
	
	cvSplit(image1,imgCanal1,imgCanal2,imgCanal3,NULL);
	
	cvMul(imgCanal1, image2, imgCanal1, 1);
	cvMul(imgCanal2, image2, imgCanal2, 1);
	cvMul(imgCanal3, image2, imgCanal3, 1);
	
	cvMerge(imgCanal1,imgCanal2,imgCanal3,NULL,res);
	
	cvReleaseImage(&imgCanal1);
	cvReleaseImage(&imgCanal2);
	cvReleaseImage(&imgCanal3);
	
	return res;
	
}

// Multiplication of a color image by a binary one
IplImage* multBinColor(IplImage *imageBin, IplImage *imageColor){
	
	IplImage *res = cvCreateImage(cvGetSize(imageColor),imageColor->depth,imageColor->nChannels);
	
	uchar* dataBin = (uchar*)imageBin->imageData;
	CvScalar coul;
	int step = imageBin->widthStep / sizeof(uchar);
	for (int i = 0; i < imageColor->width; i++){
		for (int j = 0; j < imageColor->height; j++){
			if (dataBin[j*step + i] == 255){
				coul = cvGet2D(imageColor,j,i);
				cvSet2D(res,j,i,coul);
			}
			else cvSet2D(res,j,i,cvScalar(0,0,0,0));
		}
	}
	
	return res;
}

IplImage* lowPassFilter(IplImage *image){
	IplImage* filteredImage = cvCreateImage(cvSize(image->width, image->height), image->depth, image->nChannels);
	double K[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	float t = 0;
	for (int i = 0; i< (3 * 3); ++i)
	t = t + K[i];
	for (int i = 0; i< (3 * 3); ++i)
	K[i] = K[i] / t;
	CvMat Kernel = cvMat(3, 3, CV_64FC1, K);
	cvFilter2D(image, filteredImage, &Kernel);

	return filteredImage;
}

// Recursive function to find the neighboring pixels and save them into a table
void findPointRec(vector<CvPoint3D32f> &point, CvPoint pixel, uchar* data, int step){
	if(data[pixel.y * step + pixel.x] != 0){
		CvPoint3D32f current = cvPoint3D32f(pixel.x, pixel.y, data[pixel.y * step + pixel.x]);
		point.push_back(current);
		data[pixel.y * step + pixel.x] = 0;
		
		findPointRec(point, cvPoint(pixel.x-1, pixel.y), data, step);
		findPointRec(point, cvPoint(pixel.x+1, pixel.y), data, step);
		findPointRec(point, cvPoint(pixel.x, pixel.y-1), data, step);
		findPointRec(point, cvPoint(pixel.x, pixel.y+1), data, step);
	}
}

// Call the recursive function when a green pixel is found
vector<vector<CvPoint3D32f> > findPoint(){
	vector<vector<CvPoint3D32f> > res;
	
	IplImage *imgCanal1 = cvCreateImage(cvGetSize(imageObjectHSV),imageObjectHSV->depth,1);
	IplImage *imgCanal2 = cvCreateImage(cvGetSize(imageObjectHSV),imageObjectHSV->depth,1);
	IplImage *imgCanal3 = cvCreateImage(cvGetSize(imageObjectHSV),imageObjectHSV->depth,1);
	
	cvSplit(imageObjectHSV,imgCanal1,imgCanal2,imgCanal3,NULL);
	int step = imgCanal2->widthStep / sizeof(uchar);
	uchar* data = (uchar*)imgCanal2->imageData;
	
	for (int i = 0; i < imgCanal2->width; i++){
		for (int j = 0; j < imgCanal2->height; j++){
			if (data[j*step + i] != 0){
				vector<CvPoint3D32f> point;
				findPointRec(point, cvPoint(i, j), data, step);
				if(point.size() > 30){ // threshold of the point size under which points are discarded
					res.push_back(point);
				}
			}
		}
	}
	
	
	cvReleaseImage(&imgCanal1);
	cvReleaseImage(&imgCanal2);
	cvReleaseImage(&imgCanal3);
	
	return res;
}

// Look for the centroid of each point with the intensity of the pixel as weight
vector<CvPoint> centroiding(vector<vector<CvPoint3D32f> > points){
	vector<CvPoint> res;
	
	long x;
	long y;
	long intensity;
	
	for (int i = 0; i < points.size(); i++){
		x = 0;
		y = 0;
		intensity = 0;
		for (int j = 0; j < points[i].size(); j++){
			intensity += points[i][j].z;
			x += points[i][j].x * points[i][j].z;
			y += points[i][j].y * points[i][j].z;
		}
		if (intensity != 0){
			x = (long) (x/intensity);
			y = (long) (y/intensity);
		}
		res.push_back(cvPoint((int) x, (int) y));
		// display it
		cvLine(imageObjectHSV, cvPoint((int)x-2,(int)y), cvPoint((int)x+2,(int)y), CvScalar(255,255,255,0));
		cvLine(imageObjectHSV, cvPoint((int)x,(int)y-2), cvPoint((int)x,(int)y+2), CvScalar(255,255,255,0));
		cvSaveImage("PointHSV.jpg", imageObjectHSV,0);
		cvLine(imageObjectRGB, cvPoint((int)x-2,(int)y), cvPoint((int)x+2,(int)y), CvScalar(255,255,255,0));
		cvLine(imageObjectRGB, cvPoint((int)x,(int)y-2), cvPoint((int)x,(int)y+2), CvScalar(255,255,255,0));
	}
	
	return res;
	
}

// Calculate the distance camera-target without taking into account the lens distortion
double findDistance(IplImage *image, CvPoint centroid){
	double p = abs(centroid.x - (image->width / 2));
	double z = fx*d/(p+fx*tanAlpha);
	return z;
}

// Calculate the distance camera-target taking into account the lens distortion
double findDistanceLensDistortion(IplImage *image, CvPoint centroid){
	double x = abs(centroid.x - principalPointx)/fx;/*image->width / 2*/ /*image->height / 2*/
	double y = abs(centroid.y - principalPointy)/fy;
	double xCenter = abs(principalPointx - image->width/2)/fx;
	double yCenter = abs(principalPointy - image->height/2)/fy;
	double r = pow(x,2) + pow(y,2);
	double rCenter = pow(xCenter,2) + pow(yCenter,2);
	double pdist = x*(1+k1*pow(r,2)+k2*pow(r,4)) + 2*p1*x*y + p2*(pow(r,2) + 2*pow(x,2));
	double pdistCenter = xCenter*(1+k1*pow(rCenter,2)+k2*pow(rCenter,4)) + 2*p1*xCenter*yCenter + p2*(pow(rCenter,2) + 2*pow(xCenter,2));
	double p = abs(pdist - pdistCenter)*fx;
	double z = fx*d/(p+fx*tanAlpha);
	return z;
}
