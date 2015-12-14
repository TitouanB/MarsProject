#include "headerGrille.hpp"

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

/*int hmin = 80; int hmax = 210;
 int smin = 100; int smax = 210;
 int vmin = 20; int vmax = 210;*/
/*int hmin = 20; int hmax = 100;
 int smin = 135; int smax = 255;
 int vmin = 90; int vmax = 210;*/
int hmin = 50; int hmax = 100;
int smin = 75; int smax = 250;
int vmin = 140; int vmax = 255;
/*boitebleuint hmin = 85; int hmax = 105;
 int smin = 115; int smax = 255;
 int vmin = 135; int vmax = 255;*/

int nbErosions = 2;
int nbDilatations = 2;

CvMemStorage* storage;
CvSeq* contours=0;

//calibrage
#define PI 3.14159265358979323846
//1 point extern double D = 1.415;
//extern double alpha = 0.1611;
extern double D = 5.21;
extern double d = 1;
extern double ratioPixelSizeF = 0.0013;
//extern double a0 = 0.06;

vector<double> tanAlphaT;

int main(int argc, char *argv[])
{
	char k;
	
	CvCapture *capture = cvCreateCameraCapture(1);
	
	frame = cvQueryFrame(capture);
	
	/* IMAGE
	const char *imageFile = "./NoisyGrid.png";
	const char *imageFileBis = "./GridCentroiding.png"; // to save

	frame = cvLoadImage(imageFile,CV_LOAD_IMAGE_COLOR);
	frameToSave = cvLoadImage(imageFileBis,CV_LOAD_IMAGE_COLOR);*/

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
		/* IMAGE
		frame = cvLoadImage(imageFile,CV_LOAD_IMAGE_COLOR);*/
		
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
			cvSaveImage("CaptureObjet.jpg", frame,0);
			cvSaveImage("CapturePointDetecteHSVCentroidObjet.jpg", imageObjectHSV,0);
			cvSaveImage("CapturePointDetecteRGBCentroidObjet.jpg", imageObjectRGB,0);
			
		}
		if (k=='q'){
			printf("goodbye Kate\n");
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

	cvSmooth(frame, imageFiltree, CV_BLUR,seuilFiltre,seuilFiltre,0.0,0.0);
	cvCvtColor(imageFiltree, imageHSV,CV_BGR2HSV);
	cvInRangeS(imageHSV,cvScalar(hmin, smin, vmin, 0.0),cvScalar(hmax, smax, vmax, 0.0),imageBinaire);
	cvErode(imageBinaire, imageErodee, NULL, nbErosions);
	cvDilate(imageErodee, imageDilatee, NULL, nbDilatations);
	
	//imageDilateeFiltree =  lowPassFilter(imageDilatee); FILTRE
	imageObjectRGB = multBinColor(imageDilatee, frame);
	imageObjectHSV = multBinColor(imageDilatee, imageHSV);
	
	//vector<CvPoint3D32f> vecPixelsDetected = pixelsDetectedCoordinates(imageObjectHSV, 2);
	
	//cout << "Taille vecPixelsDetected : " << vecPixelsDetected.size() << endl;
	
	//cout << "coord alea : x " << res[10].x << " y " << res[10].y << " z " << res[10].z << endl;
	//vector<vector<CvPoint3D32f> > vecDistinctPoints = pointsDistinction(vecPixelsDetected);
	vector<vector<CvPoint3D32f> > vecDistinctPoints = findPoint();
	
	//cout << "Taille : " << vecDistinctPoints.size() << " x " << vecDistinctPoints[0].size() << endl;
	
	// find the centroid of the object and trace it
	vector<CvPoint> centroid = centroiding(vecDistinctPoints);
	sort(centroid);
	
	vector<double> distance2 = findDistance2(imageObjectHSV, centroid, tanAlphaT);
	
	// Contours
	/*cvFindContours( imageDilatee, storage, &contours, sizeof(CvContour),
				   CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0,0) );*/
	
	//imageFinale = multiplier(frame/*imageHSV*/, imageDilatee);
	//cvDrawContours( imageFinale, contours,
	/*cvDrawContours( frame, contours,
				   CV_RGB(255,255,0), CV_RGB(0,255,0),
				   1, 2, 8, cvPoint(0,0));*/
	
	//cvCvtColor(imageFinale, imageFinale,CV_HSV2BGR);
	
	
	cvNamedWindow(myWindow, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(myWindowObjectHSV, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(myWindowObjectRGB, CV_WINDOW_AUTOSIZE);
	/*cvResizeWindow(myWindowObjectHSV, 500, 400);
	 cvResizeWindow(myWindowObjectRGB, 500, 400);
	 cvMoveWindow(myWindowObjectHSV, 0, 0);
	 cvMoveWindow(myWindowObjectRGB, 515, 0);*/
	//cvShowImage(myWindow, imageFinale);
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

void sort(vector<CvPoint>& tab){
	bool sortedTab = false;
	long size = tab.size();
	while(!sortedTab)
	{
		sortedTab = true;
		for(int i=0 ; i < size-1 ; i++)
		{
			if(tab[i].x > tab[i+1].x)
			{
				swap(tab[i],tab[i+1]);
				sortedTab = false;
			}
		}
		size--;
	}
}



/*double findDistance(IplImage *image, int xy[]){
	int A0 = image->width / 2;
	double d = D - (abs(xy[0] - image->width / 2) * a0 / (A0*tan(alpha)));
	printf("distance camera-target = %f\n", d);
	return d;
}*/

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
		//cout << "alpha " << i << " : " << tanAlphaT[i] << " ";
	}
	//cout << endl;
	//cout << "alpha atan " << atan(d/D) << endl;
	return tanAlphaT;
}

vector<double> findDistance2(IplImage *image, vector<CvPoint> centroid, vector<double> tanAlphaT){

	double p;
	vector<double> z = vector<double>(centroid.size(),0);

	for (int i=0; i<centroid.size(); i++){
		p = abs(centroid[i].x - (frame->width / 2));
		z[i] = d/(p*ratioPixelSizeF+tanAlphaT[i]);
		cout << "centroid " << i << " : " << centroid[i].x << " x " << centroid[i].y << " ";
		cout << "z " << i << " : " << z[i] << " ";
	}
	cout << endl;
	
	return z;
}




/*
 vector<CvPoint3D32f> pixelsDetectedCoordinates(IplImage *image, int canal){
	vector<CvPoint3D32f> res;
	
	// canal : 1 blue, 2 green, 3 red RGB
	// canal : 1 hue, 2 saturation, 3 value HSV
	IplImage *imgCanal1 = cvCreateImage(cvGetSize(image),image->depth,1);
	IplImage *imgCanal2 = cvCreateImage(cvGetSize(image),image->depth,1);
	IplImage *imgCanal3 = cvCreateImage(cvGetSize(image),image->depth,1);
	
	cvSplit(image,imgCanal1,imgCanal2,imgCanal3,NULL);
	
	IplImage *imgCanal = cvCreateImage(cvGetSize(image),image->depth,1);
	if (canal == 1) imgCanal = imgCanal1;
	else if (canal == 2) imgCanal = imgCanal2;
	else imgCanal = imgCanal3;
	
	uchar* data = (uchar*)imgCanal->imageData;
	int step = imgCanal->widthStep / sizeof(uchar);
	for (int i = 0; i < imgCanal->width; i++){
 for (int j = 0; j < imgCanal->height; j++){
 if (data[j*step + i] != 0){
 res.push_back(cvPoint3D32f(i, j, data[j*step + i]));
 }
 }
	}
 
	cvReleaseImage(&imgCanal1);
	cvReleaseImage(&imgCanal2);
	cvReleaseImage(&imgCanal3);
 
	return res;
 }
 
 vector<vector<CvPoint3D32f> > pointsDistinction(vector<CvPoint3D32f> vec){
	vector<vector<CvPoint3D32f> > res(0);
	vector<CvPoint3D32f> tmpIni(1,vec[0]);
	res.push_back(tmpIni); // first pixel belongs to the first point
	
	// Each detected pixel is moved into a new tab where the rows correspond to points and the colomns to pixels belonging to the point
	for (int i=1; i<vec.size(); i++){
 int j = 0;
 bool belongsToPoint = false;
 while (!belongsToPoint && j<res.size()){
 if (searchNeighboursPixel(res[j], vec[i])){
 res[j].push_back(vec[i]);
 belongsToPoint = true;
 }
 else j++;
 }
 if (!belongsToPoint){
 vector<CvPoint3D32f> tmp(1,vec[i]);
 res.push_back(tmp);
 }
 
	}
	
	cout << "res : " << res.size() << endl;
	
	// Pixels belonging to the same point are gathered in the same row
	while(res.size() > 4) {
	for (int i=1; i<res.size(); i++){ // we begin to the second point/row
 int j = 0;
 while (j<i){
 if (searchNeighboursVect(res[j], res[i])){
 res[j].insert(res[j].end(), res[i].begin(), res[i].end());
 res.erase(res.begin()+i);
 break;
 }
 else j++;
 }
	}
	}
 
	
	return res;
 }
 
 bool searchNeighboursPixel(vector<CvPoint3D32f> vec, CvPoint3D32f pix){
	bool findNeighbours = false;
 
	int j = 0;
 
	while(!findNeighbours && j<vec.size()){
 if ((pix.x == vec[j].x && pix.y - 1 == vec[j].y) ||
 (pix.x == vec[j].x && pix.y + 1 == vec[j].y) ||
 (pix.x - 1 == vec[j].x && pix.y == vec[j].y) ||
 (pix.x + 1 == vec[j].x && pix.y == vec[j].y))
 findNeighbours = true;
 j++;
	}
	
	return findNeighbours;
 }
 
 bool searchNeighboursVect(vector<CvPoint3D32f> vecPoint1, vector<CvPoint3D32f> vecPoint2){
	// vecPoint1: reference vector
	bool findNeighbours = false;
	
	int i = 0, j = 0;
	while (!findNeighbours && i<vecPoint2.size()){
 while (!findNeighbours && j<vecPoint1.size()){
 if ((vecPoint2[i].x == vecPoint1[j].x && vecPoint2[i].y - 1 == vecPoint1[j].y) ||
 (vecPoint2[i].x == vecPoint1[j].x && vecPoint2[i].y + 1 == vecPoint1[j].y) ||
 (vecPoint2[i].x - 1 == vecPoint1[j].x && vecPoint2[i].y == vecPoint1[j].y) ||
 (vecPoint2[i].x + 1 == vecPoint1[j].x && vecPoint2[i].y == vecPoint1[j].y))
 findNeighbours = true;
 j++;
 }
 i++;
	}
	
	return findNeighbours;
 }
*/