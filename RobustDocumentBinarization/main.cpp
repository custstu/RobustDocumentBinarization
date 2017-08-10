/*implementation of paper "Robust document binarization with OFF center-surround cells"
@Author:Tom Liao 8/9/2017*/
#include <iostream>
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;
/*
function:
*binarization with OFF center-surround cells
input:
*[in]Mat srcImage - original image.
*[in]int surroundSize - surround filter window size.
*[in]int centerSize - center filter window size.
output:
*[out]Mat &dstImage - binarizationed image.
return:
-1 - invalid, 0 - valid
*/
int binarization(Mat srcImage, int surroundSize, int centerSize, Mat &dstImage)
{
	if (surroundSize % 2 == 0 || centerSize % 2 == 0)
	{
		return -1;
	}
	Mat grayImage, stretchImage;

	if (srcImage.channels() == 3)
	{
		cvtColor(srcImage, grayImage, CV_BGR2GRAY);
	}
	else
	{
		grayImage = srcImage;
	}
	double minVal[2], maxVal[2];
	minMaxIdx(grayImage, minVal, maxVal);

	//Pre-Processing 
	stretchImage = (grayImage - minVal[0]) * 255 / (maxVal[0] - minVal[0]);

	//Receptive fields
	Mat surround, center;
	stretchImage.convertTo(stretchImage, CV_32FC1);
	blur(stretchImage, surround, Size(surroundSize, surroundSize));
	blur(stretchImage, center, Size(centerSize, centerSize));

	//Response
	Mat sc, g, gUchar;
	g = (surround + 255.0).mul(surround - center) / (surround * 2 - center);
	sc = surround - center;
	for (int i = 0; i < sc.cols * sc.rows; i++)
	{
		if (((float*)sc.data)[i] <= 0)
		{
			((float*)g.data)[i] = 0;
		}
	}
	g.convertTo(gUchar, CV_8UC1);
	threshold(gUchar, dstImage, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
	dstImage = ~dstImage;
	return 0;
}


//binarization test
int main()
{
	Mat srcImage = imread("2.png");
	namedWindow("srcImage", 1);
	imshow("srcImage", srcImage);
	waitKey(0);
	Mat dstImage;
	int ret = binarization(srcImage, 31, 3, dstImage);
	if (ret == -1)
	{
		return 0;
	}
	namedWindow("dstImage", 1);
	imshow("dstImage", dstImage);
	waitKey(0);
	destroyAllWindows();
	return 0;
}