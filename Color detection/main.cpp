#include "header.h"

IplImage *frame = 0;
IplImage* imageFiltree;
IplImage* imageHSV;
IplImage* imageBinaire;
IplImage* imageErodee;
IplImage* imageDilatee;
IplImage* imageDilateeFiltree;
IplImage* imageObject;
IplImage* imageFinale;

int seuilFiltre = 1;

const char *myWindow = "Flux Webcam";
const char *myWindowObject = "Detected Object";

/*int hmin = 80; int hmax = 210;
int smin = 100; int smax = 210;
int vmin = 20; int vmax = 210;*/
/*int hmin = 20; int hmax = 100;
int smin = 135; int smax = 255;
int vmin = 90; int vmax = 210;*/
int hmin = 85; int hmax = 105;
int smin = 115; int smax = 255;
int vmin = 0; int vmax = 140;

int nbErosions = 0;
int nbDilatations = 0;

CvMemStorage* storage;
CvSeq* contours=0;


int main(int argc, char *argv[])
{
	char k;
	
	CvCapture *capture = cvCreateCameraCapture(0);
	
	
	frame = cvQueryFrame(capture);
	
	imageFiltree = cvCreateImage(cvGetSize(frame),frame->depth,frame->nChannels);
	imageHSV = cvCreateImage(cvGetSize(frame),frame->depth,frame->nChannels);
	imageBinaire = cvCreateImage(cvGetSize(frame),frame->depth,1);
	imageErodee = cvCreateImage(cvGetSize(frame),frame->depth,1);
	imageDilatee = cvCreateImage(cvGetSize(frame),frame->depth,1);
	imageDilateeFiltree = cvCreateImage(cvGetSize(frame),frame->depth,1);
	imageObject = cvCreateImage(cvGetSize(frame),frame->depth,frame->nChannels);
	imageFinale = cvCreateImage(cvGetSize(frame),frame->depth,frame->nChannels);
	
	storage = cvCreateMemStorage(0);
	
	for (;;)
	{
		frame = cvQueryFrame(capture);
		
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
			printf("sauvegarde\n");
			cvSaveImage("Capture.jpg", frame,0);
			
		}
		if (k=='q'){
			printf("goodbye Kate\n");
			break;
		}
	}
	cvDestroyWindow(myWindowObject);
	cvDestroyWindow(myWindow);
	cvReleaseCapture(&capture);
	cvReleaseImage(&imageFiltree);
	cvReleaseImage(&imageBinaire);
	cvReleaseImage(&imageErodee);
	cvReleaseImage(&imageDilatee);
	cvReleaseImage(&imageDilateeFiltree);
	cvReleaseImage(&imageObject);
	cvReleaseImage(&imageFinale);
	cvReleaseMemStorage(&storage);
}

void callback(int i)
{
	cvSmooth(frame, imageFiltree, CV_BLUR,seuilFiltre,seuilFiltre,0.0,0.0);
	cvCvtColor(imageFiltree, imageHSV,CV_BGR2HSV);
	cvInRangeS(imageHSV,cvScalar(hmin, smin, vmin, 0.0),cvScalar(hmax, smax, vmax, 0.0),imageBinaire);
	cvErode(imageBinaire, imageErodee, NULL, nbErosions);
	cvDilate(imageErodee, imageDilatee, NULL, nbDilatations);
	
	//imageDilateeFiltree =  lowPassFilter(imageDilatee); FILTRE
	imageObject = multBinRGB(imageDilatee, frame);
	
	// Contours
	cvFindContours( imageDilatee, storage, &contours, sizeof(CvContour),
				   CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0,0) );
	
	//imageFinale = multiplier(frame/*imageHSV*/, imageDilatee);
	//cvDrawContours( imageFinale, contours,
	cvDrawContours( frame, contours,
				   CV_RGB(255,255,0), CV_RGB(0,255,0),
				   1, 2, 8, cvPoint(0,0));
	
	//cvCvtColor(imageFinale, imageFinale,CV_HSV2BGR);
	
	cvNamedWindow(myWindow, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(myWindowObject, CV_WINDOW_AUTOSIZE);
	//cvShowImage(myWindow, imageFinale);
	cvShowImage(myWindow, frame);
	cvShowImage(myWindowObject, imageObject);
	
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

IplImage* multBinRGB(IplImage *imageBin, IplImage *imageRGB){
	
	IplImage *res = cvCreateImage(cvGetSize(imageRGB),imageRGB->depth,imageRGB->nChannels);
	
	uchar* dataBin = (uchar*)imageBin->imageData;
	CvScalar coul;
	int step = imageBin->widthStep / sizeof(uchar);
	for (int i = 0; i < imageRGB->width; i++){
		for (int j = 0; j < imageRGB->height; j++){
			if (dataBin[j*step + i] == 255){
				coul = cvGet2D(imageRGB,j,i);
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