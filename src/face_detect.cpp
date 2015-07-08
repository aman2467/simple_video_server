#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <face.h>

using namespace std;
using namespace cv;

vector<Rect> faces; 
CascadeClassifier face_cascade;

int face_detect_init(void)
{
	if(!face_cascade.load("utils/frontal_face_detection.xml")) {
		printf("Error: load fail\n");
		return -1;
	}
}

int face_detect_draw(unsigned char *frame, int width, int height)
{
	Mat f, f_g;
	#if 1
	f.data = frame;
	f.flags = 1124024336;
	f.dims = 2;
	f.rows = height;
	f.cols = width;
	#else
	f = Mat(height, width, CV_16UC4 ,0);
	#endif
	cvtColor(f, f_g, CV_BGR2GRAY);
	equalizeHist(f_g, f_g);
	face_cascade.detectMultiScale(f_g, faces,
			1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));

	for(size_t i = 0; i < faces.size(); i++) {
		Point center(faces[i].x + faces[i].width*0.5,
				faces[i].y + faces[i].height*0.5);
		printf("running\n");
		ellipse( f, center,
				Size(faces[i].width*0.5, faces[i].height*0.5), 
				0, 0, 360, Scalar(255, 255, 255), 4, 8, 0);
	}
	return 0;
}
