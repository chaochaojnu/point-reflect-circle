#include <iostream>
#include <opencv2\opencv.hpp>


using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
	Mat src = imread("..\\data\\1.bmp", 1);//输入图
	Mat srcdisplay = Mat::zeros(src.rows, src.cols, CV_8UC1);
	vector<Mat> channels;
	split(src, channels);
	Mat heightMapRed = channels.at(2);
	//1检测圆心半径
	Mat srcgray;
	cvtColor(src, srcgray, COLOR_RGB2GRAY);
	double circle_R = 0;
	double perimeter = 0;
	double circlearea = 0;
	vector<Vec3f> circles;
	Point circle_center;
	HoughCircles(srcgray, circles, HOUGH_GRADIENT, 1, 200, 100, 30, 800, 1200);
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center((circles[i][0]), (circles[i][1]));
		int radius = (circles[i][2]);
		if (center.x <2200 && center.x > 1800 && center.y < 1900 && center.y > 1500 && radius > 900 && radius < 1100)
		{
			circle_R = radius + 12;
			circle_center = center;
			cout << "圆心X=" << center.x << "   " << "圆心Y=" << center.y << "   " << "半径R=" << radius << endl;
			circle(src, center, 3, Scalar(255, 255, 255), -1, 8, 0);
			perimeter = 6.28 * circle_R;//完整圆环周长
			circlearea = 3.14 * circle_R * circle_R;
		}
	}
	//2读取坐标文件
	vector <Point> circlepoint;
	for (int x = 0; x < srcgray.rows; x++)
	{
		for (int y = 0; y < srcgray.cols; y++)
		{
			if (heightMapRed.at<uchar>(x, y) == 255)
				circlepoint.push_back(Point(y, x));
		}
	}
	//3求圆和线交点
	vector <Point> reflectepoint;
	double k = 0;
	double b = 0;
	double a = 0;
	double b1 = 0;
	double c = 0;
	double dlta = 0;
	double x1, y1, x2, y2;
	for (int i = 0; i < circlepoint.size(); i++)
	{
		if (circlepoint[i].x < circle_center.x)
		{
			k = (double)(circlepoint[i].y - circle_center.y) / (circlepoint[i].x - circle_center.x);
			b = (circle_center.y - k * circle_center.x);
			a = k * k + 1;
			b1 = 2 * k * b - 2 * k * circle_center.y - 2 * circle_center.x;
			c = circle_center.x * circle_center.x - circle_R * circle_R + (b - circle_center.y) * (b - circle_center.y);
			dlta = b1 * b1 - 4 * a * c;
			if (dlta >= 0)
			{
				x1 = (-b1 - sqrt(dlta)) / (2 * a);
				x2 = (-b1 + sqrt(dlta)) / (2 * a);
				y1 = k * x1 + b;
				y2 = k * x2 + b;
				if (x1 < circle_center.x)
				{
					reflectepoint.push_back(Point(x1, y1));
				}
				if (x2 < circle_center.x)
				{
					reflectepoint.push_back(Point(x2, y2));
				}
			}
		}
		else if (circlepoint[i].x > circle_center.x)
		{
			k = (double)(circlepoint[i].y - circle_center.y) / (circlepoint[i].x - circle_center.x);
			b = (circle_center.y - k * circle_center.x);
			a = k * k + 1;
			b1 = 2 * k * b - 2 * k * circle_center.y - 2 * circle_center.x;
			c = circle_center.x * circle_center.x - circle_R * circle_R + (b - circle_center.y) * (b - circle_center.y);
			dlta = b1 * b1 - 4 * a * c;
			if (dlta >= 0)
			{
				x1 = (-b1 - sqrt(dlta)) / (2 * a);
				x2 = (-b1 + sqrt(dlta)) / (2 * a);
				y1 = k * x1 + b;
				y2 = k * x2 + b;
				if (x1 > circle_center.x)
				{
					reflectepoint.push_back(Point(x1, y1));
				}
				if (x2 > circle_center.x)
				{
					reflectepoint.push_back(Point(x2, y2));
				}
			}
		}
		else
		{
			if (circlepoint[i].y < circle_center.y)
			{
				reflectepoint.push_back(Point(circlepoint[i].x, circle_center.y - circle_R));
			}
			if (circlepoint[i].y > circle_center.y)
			{
				reflectepoint.push_back(Point(circlepoint[i].x, circle_center.y + circle_R));
			}
		}
	}
	//4画出映射点
	for (int i = 0; i < reflectepoint.size(); i++)
	{
		line(src, reflectepoint[i], reflectepoint[i], Scalar(255, 255, 255), 3);
	}
	//imwrite("1.jpg", src);
	for (int i = 0; i < reflectepoint.size(); i++)
	{
		line(srcdisplay, reflectepoint[i], circle_center, Scalar(255, 255, 255), 1);
		line(src, reflectepoint[i], circle_center, Scalar(255, 255, 255), 1);

	}

	//5预处理 
	Mat element = getStructuringElement(MORPH_RECT, Size(7, 7));
	dilate(src, src, element);
	//imwrite("2.jpg", src);
	dilate(srcdisplay, srcdisplay, element);
	int bowarea = 0;
	int temp = 0;
	for (int x = 0; x < srcdisplay.rows; x++)
	{
		for (int y = 0; y < srcdisplay.cols; y++)
		{
			temp = srcdisplay.at<uchar>(x, y);
			if (temp == 255)
				bowarea++;
		}
	}
	double factor = bowarea / circlearea;
	cout << factor << endl;
}

