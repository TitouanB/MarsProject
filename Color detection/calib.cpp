#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <cv.hpp>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>


const char *myWindow = "Detected Object";

int main(int argc, char *argv[])
{
	char k;
	
	CvCapture *capture = cvCreateCameraCapture(1);
	IplImage* frame = 0;
	
	frame = cvQueryFrame(capture);
	
	
	for (;;)
	{
		frame = cvQueryFrame(capture);
		
		//cvSmooth(frame, imageFiltree, CV_BLUR,seuilFiltre,seuilFiltre,0.0,0.0);
		
		if (!frame)
			break;
		
		cvLine(frame, cvPoint(frame->width/2 - 4, frame->height/2), cvPoint(frame->width/2 + 4, frame->height/2), CvScalar(0,0,0,0));
		cvLine(frame, cvPoint(frame->width/2, frame->height/2 - 4), cvPoint(frame->width/2, frame->height/2 + 4), CvScalar(0,0,0,0));
		
		cvNamedWindow(myWindow, CV_WINDOW_AUTOSIZE);
		cvShowImage(myWindow, frame);

		int delay = 10;
		k=cvWaitKey(delay);
		
		
		if(k=='s'){
			printf("sauvegarde\n");
			cvSaveImage("CaptureFocalLength.jpg", frame,0);
			
		}
		if (k=='q'){
			printf("goodbye Kate\n");
			break;
		}
	}

	cvDestroyWindow(myWindow);
	cvReleaseCapture(&capture);

}