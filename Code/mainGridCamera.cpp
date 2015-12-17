#include "headerGridCamera.hpp"

IplImage* frame = 0;
IplImage* frameToSave;
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

int hmin = 50; int hmax = 100;
int smin = 75; int smax = 250;
int vmin = 140; int vmax = 255;

int nbErosions = 4;
int nbDilatations = 4;

CvMemStorage* storage;
CvSeq* contours=0;

bool grid;

// calibration
#define PI 3.14159265358979323846
double D = 5.21;
double d = 1;
double ratioPixelSizeF = 0.0013;

vector<double> tanAlphaT;

int main(int argc, char *argv[])
{
	char k;
	
	// CAM
	CvCapture *capture = cvCreateCameraCapture(0);
	grid = false;  // true if the pattern is a grid, false if it is a line
	
	frame = cvQueryFrame(capture);
	
	// IMAGE
	/*const char *imageFile = "./NoisyGrid.png";
	const char *imageFileBis = "./GridCentroiding.png"; // to save
	grid = false;
	 
	frame = cvLoadImage(imageFile,CV_LOAD_IMAGE_COLOR);
	frameToSave = cvLoadImage(imageFileBis,CV_LOAD_IMAGE_COLOR);*/
	
	// Initialization
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
	
	cvSaveImage("frame.jpg", frame,0);
	//tanAlphaT = calibrate();
	tanAlphaT.push_back(-0.174178);
	tanAlphaT.push_back(-0.132284);
	tanAlphaT.push_back(-0.0912077);
	tanAlphaT.push_back(-0.0511169);
	tanAlphaT.push_back(-0.00956113);
	tanAlphaT.push_back(0.0320276);
	tanAlphaT.push_back(0.0722128);
	tanAlphaT.push_back(0.110882);
	tanAlphaT.push_back(0.150492);
	tanAlphaT.push_back(0.188378);
	
	for (;;)
	{
		// IMAGE
		//frame = cvLoadImage(imageFile,CV_LOAD_IMAGE_COLOR);
		
		// CAM
		frame = cvQueryFrame(capture);
		
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
			cvSaveImage("CaptureObjet.jpg", frame,0);
			cvSaveImage("CapturePointDetecteHSVCentroidObjet.jpg", imageObjectHSV,0);
			cvSaveImage("CapturePointDetecteRGBCentroidObjet.jpg", imageObjectRGB,0);
			
		}
		if (k=='q'){
			printf("Goodbye\n");
			break;
		}
	}
	cvDestroyWindow(myWindowObjectHSV);
	cvDestroyWindow(myWindowObjectRGB);
	cvDestroyWindow(myWindow);
	cvReleaseCapture(&capture);
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
	
	// find the points and separate them (rows correspond to each point and the columns to the pixels belonging to the points)
	vector<vector<CvPoint3D32f> > vecDistinctPoints = findPoint();
	
	// find the centroid of the point and trace it
	vector<CvPoint> centroid = centroiding(vecDistinctPoints);
	// sort the centroids
	centroid = sort(centroid);
	
	// compute the distance with and without lens distortion
	vector<double> distance = findDistance(imageObjectHSV, centroid, tanAlphaT);
	
	// Contours
	/*cvFindContours( imageDilatee, storage, &contours, sizeof(CvContour),
	 CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0,0) );*/
	
	/*cvDrawContours( frame, contours,
	 CV_RGB(255,255,0), CV_RGB(0,255,0),
	 1, 2, 8, cvPoint(0,0));*/
	
	
	cvNamedWindow(myWindow, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(myWindowObjectHSV, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(myWindowObjectRGB, CV_WINDOW_AUTOSIZE);
	cvShowImage(myWindow, frame);
	cvShowImage(myWindowObjectHSV, imageObjectHSV);
	cvShowImage(myWindowObjectRGB, imageObjectRGB);
	//cvSaveImage("NoisyGridCentroiding.png", imageObjectRGB,0);
	
	// End timer
	t2 = clock();
	
	// Compute execution time
	time = (float)(t2 - t1) / CLOCKS_PER_SEC;
	
	cout << "execution time = " << time << " s" << endl;
	
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
			x = (int) (x/intensity);
			y = (int) (y/intensity);
		}
		res.push_back(cvPoint((int) x, (int) y));
		// display it
		cvLine(imageObjectHSV, cvPoint((int)x-2,(int)y), cvPoint((int)x+2,(int)y), CvScalar(255,255,255,0));
		cvLine(imageObjectHSV, cvPoint((int)x,(int)y-2), cvPoint((int)x,(int)y+2), CvScalar(255,255,255,0));
		cvLine(imageObjectRGB, cvPoint((int)x-2,(int)y), cvPoint((int)x+2,(int)y), CvScalar(255,255,255,0));
		cvLine(imageObjectRGB, cvPoint((int)x,(int)y-2), cvPoint((int)x,(int)y+2), CvScalar(255,255,255,0));
		cvLine(frameToSave, cvPoint((int)x-2,(int)y), cvPoint((int)x+2,(int)y), CvScalar(255,0,0,0));
		cvLine(frameToSave, cvPoint((int)x,(int)y-2), cvPoint((int)x,(int)y+2), CvScalar(255,0,0,0));
	}
	
	return res;
	
}

// sort the centroids table
vector<CvPoint> sort(vector<CvPoint> tab){
	int nbPointsPerRow;
	int nbPointsPerColumn;
	if (grid) {
		nbPointsPerColumn = sqrt(tab.size());
		nbPointsPerRow = nbPointsPerColumn;
	}
	else {
		nbPointsPerColumn = 1;
		nbPointsPerRow = tab.size();
	}
	
	vector<CvPoint> res = vector<CvPoint>(tab.size(),cvPoint(frame->width,0));
	int compteur = 0;
	int index = 0;
	
	for (int i=0; i<nbPointsPerColumn ; i++){
		for (int j=0; j<nbPointsPerRow ; j++){
			compteur = 0;
			index=0;
			while (compteur < tab.size()){
				if(tab[index].y>tab[compteur].y){
					index = compteur;
				}
				compteur++;
			}
			insert(res, tab[index], i);
			tab.erase(tab.begin()+index);
		}
	}
	
	return res;
}

void insert(vector<CvPoint> &tab, CvPoint point, int i){
	int compteur = i*10;
	while (compteur < (i+1)*10 && tab[compteur].x<point.x) {
		compteur ++;
	}
	CvPoint tmp;
	while (compteur < (i+1)*10 ){
		tmp = tab[compteur];
		tab[compteur]=point;
		point=tmp;
		compteur++;
	}
}

// calibration function to be run at the beginning only
vector<double> calibrate(){
	
	cvSmooth(frame, imageFiltree, CV_BLUR,seuilFiltre,seuilFiltre,0.0,0.0);
	cvCvtColor(imageFiltree, imageHSV,CV_BGR2HSV);
	cvInRangeS(imageHSV,cvScalar(hmin, smin, vmin, 0.0),cvScalar(hmax, smax, vmax, 0.0),imageBinaire);
	cvErode(imageBinaire, imageErodee, NULL, nbErosions);
	cvDilate(imageErodee, imageDilatee, NULL, nbDilatations);
	
	imageObjectRGB = multBinColor(imageDilatee, frame);
	imageObjectHSV = multBinColor(imageDilatee, imageHSV);
	
	vector<vector<CvPoint3D32f> > vecDistinctPoints = findPoint();
	
	// find the centroid of the object and trace it
	vector<CvPoint> centroid = centroiding(vecDistinctPoints);
	sort(centroid);
	
	vector<double> tanAlphaT = vector<double>(centroid.size(),0);
	double p;
	
	for (int i=0; i<centroid.size(); i++){
		p = abs(centroid[i].x - (frame->width / 2));
		tanAlphaT[i] = atan(d/D-p*ratioPixelSizeF);
	}
	return tanAlphaT;
}

// Calculate the distance camera-target without taking into account the lens distortion
vector<double> findDistance(IplImage *image, vector<CvPoint> centroid, vector<double> tanAlphaT){
	
	double p;
	vector<double> z = vector<double>(centroid.size(),0);
	
	for (int i=0; i<centroid.size(); i++){
		p = abs(centroid[i].x - (frame->width / 2));
		z[i] = d/(p*ratioPixelSizeF+tanAlphaT[i]);
	}
	
	return z;
}