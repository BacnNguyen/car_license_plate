#include<opencv2\opencv.hpp>
using namespace std;
using namespace cv;
bool checkPossibleChar(Rect r) {
	int x = r.x;
	int y = r.y;
	int width = r.width;
	int height = r.height;
	int s = width * height;
	//cout << "s = " << s << "\n";
	if (s < 300) return false;
	if (s >900) return false;
	if (height / width < 1 || height / width>4) {
		return false;
	}
	
	return true;
}

bool checkPossibleChar1(Rect r) {
	int x = r.x;
	int y = r.y;
	int width = r.width;
	int height = r.height;
	int s = width * height;
	//cout << "s = " << s << "\n";
	if (s < 300) return false;
	if (s >1500) return false;
	if (height / width < 1 || height / width>4) {
		return false;
	}

	return true;
}
bool checkPossiblePlate(Rect r) {
	int width = r.width;
	int height = r.height;
	int s = width*height;
	
	if (s < 1500) return false;
	if (s > 4000) return false;
	/*if (width < 30 || width>100) return false;
	if (height < 10 || height>50) return false;*/
	if (width / height < 1.5 ||width/height>2.5 ) {
		return false;
	}
	return true;
}

bool checkPossiblePlate1(Rect r) {
	int width = r.width;
	int height = r.height;
	int s = width*height;

	if (s < 3000) return false;
	if (s >35000) return false;
	return true;
}
int myfunc(Rect r, Rect r1) {
	return r.x>r1.x?0:1;
}

vector<Rect> checkPlates(Mat plt) {
	if (plt.size().width*plt.size().height < 22000) {
		resize(plt, plt, Size(220, 100));
	}
	cout << "Size :"<<plt.size() << "\n";
	blur(plt, plt, Size(3, 3));
	Mat thresh;
	threshold(plt, thresh, 100, 250, THRESH_BINARY_INV | THRESH_OTSU);
	imshow("Thresh", thresh);
	int erode_size =1;
	int dilate_size = 1;
	Mat kernel_dilate = getStructuringElement(MORPH_RECT,
		Size(2 * dilate_size + 1, 2 * dilate_size + 1),
		Point(0, 0));
	Mat kernel_erode = getStructuringElement(MORPH_RECT,
		Size(2 * erode_size + 1, 2 * erode_size + 1),
		Point(0, 0));



	dilate(thresh, thresh, kernel_dilate);
	erode(thresh, thresh, kernel_erode);
	/*erode(thresh, thresh, kernel_erode);
	dilate(thresh, thresh, kernel_dilate);*/

	
	
	
	imshow("Thresh1", thresh);
	Mat canny;
	Canny(thresh, canny, 100, 250);
	//imshow("Canny", canny);
	vector<vector<Point> > contours;
	findContours(canny, contours, RETR_TREE, CHAIN_APPROX_NONE);
	vector<Rect> chars;
	Mat empty = Mat::zeros(canny.size(), CV_8UC3);
	Mat drawing = Mat::zeros(canny.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++) {
		//drawContours(drawing, contours, i, Scalar(0, 255, 0), 3);
		Rect r = boundingRect(contours[i]);
		if (checkPossibleChar(r)||checkPossibleChar1(r)) {
			//cout << empty.at<int>(r.y, r.x) << "\n";
			if (empty.at<int>(r.y, r.x) == 0) {
				empty.at<int>(r.y, r.x) = 255;
				chars.push_back(r);
				rectangle(drawing, r, Scalar(0, 255, 0));
				/*circle(empty, Point(r.x, r.y), 3, Scalar(0, 0, 255));*/
			}
		}
	}
	/*if (chars.size() > 4) {
		cout << chars.size() << "\n";
	}*/
	//imshow("Contour", drawing);
	if (chars.size() > 3) {
		sort(chars.begin(), chars.end(), myfunc);
		for (int j = 0; j < chars.size(); j++) {
			//cout <<"j = "<<j<<":"<< chars[j].x << "\n";
			Mat temp = thresh(chars[j]);
			char name[50];
			sprintf_s(name, "name %d", j);
			imshow(name, temp);
		}
		waitKey(0);
	}
	
	return chars;
}



void process(Mat img) {
	if (img.size().width > 1100 || img.size().width < 900||img.size().height<550||img.size().height>650) {
		resize(img, img, Size(1000, 600));
	}
	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	//imshow("Gray", gray);
	equalizeHist(gray, gray);
	//imshow("Gray1", gray);

	Mat blur_img;
	blur(gray, blur_img, Size(3, 3));
	Mat thresh;
	threshold(blur_img, thresh, 50, 200, THRESH_BINARY| THRESH_OTSU);
	//imshow("threshold", thresh);
	int dilate_size = 2;
	Mat element = getStructuringElement(MORPH_RECT,
		Size(2 * dilate_size + 1, 2 * dilate_size + 1),
		Point(dilate_size, dilate_size));
	int erode_size = 1;
	Mat element1 = getStructuringElement(MORPH_RECT,
		Size(2 * erode_size + 1, 2 * erode_size + 1),
		Point(erode_size, erode_size));
	dilate(thresh, thresh, element);
	erode(thresh, thresh, element1);
	imshow("threshold1", thresh);

	Mat canny;
	Canny(thresh, canny, 100, 250);
	imshow("Canny", canny);
	vector<vector<Point> > contours;

	vector<Vec4i> hierarchy;
	findContours(canny, contours, RETR_TREE, CHAIN_APPROX_NONE);
	Mat drawing = Mat::zeros(canny.size(), CV_8UC3);
	Mat bienso;
	vector<Mat> plates;
	for (int i = 0; i < contours.size(); i++) {
		//drawContours(drawing, contours, i, Scalar(0, 255, 0), 3);
		Rect r = boundingRect(contours[i]);
		if (checkPossiblePlate1(r)||checkPossiblePlate(r)) {
			cout << "area =" << r.width*r.height << "\n";
			//cout << "w :" << r.width << "------h :" << r.height << "\n";
			rectangle(drawing, r, Scalar(0, 0, 255));
			plates.push_back(gray(r));
		}
	}
	//imshow("bienso", bienso);
	imshow("Contours", drawing);
	cout << "Number of plates:"<<plates.size()<<"\n";
	vector<Rect> chars;
	for (int i = 0; i < plates.size(); i++) {
		/*imshow("Plate", plates[i]);
		waitKey(0);*/
		chars = checkPlates(plates[i]);
	}

	////blur(bienso, bienso, Size(3, 3));
	//Mat thresh_bienso;
	//threshold(bienso, thresh_bienso, 100, 250, THRESH_BINARY_INV | THRESH_OTSU);
	//imshow("Thresh Bienso", thresh_bienso);
	//int dilate_size = 1;
	//Mat element = getStructuringElement(MORPH_RECT,
	//	Size(2 * dilate_size + 1, 2 * dilate_size + 1),
	//	Point(dilate_size, dilate_size));
	//dilate(thresh_bienso, thresh_bienso, element);
	//erode(thresh_bienso, thresh_bienso, element);
	//imshow("Thresh Bienso 1", thresh_bienso);

	//Mat canny_bs;
	//Canny(thresh_bienso, canny_bs, 100, 250);
	//imshow("Canny BS", canny_bs);

	//vector<vector<Point> > contour;
	//findContours(canny_bs, contour, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	//cout << "Contour size:" << contour.size();
	//Mat mask = Mat::zeros(bienso.size(), CV_8UC3);
	//vector<Mat> chars;
	//for (int i = 0; i < contour.size(); i++) {
	//	Rect r = boundingRect(contour[i]);
	//	if (checkPossibleChar(r)) {
	//		rectangle(mask, r, Scalar(0, 255, 0), 1);
	//		cout << "Width:" << r.width << "| Height:" << r.height << "\n";
	//		chars.push_back(bienso(r));
	//	}

	//	/*if (r.width > 5 && r.height > 5) {
	//		float scale = r.height / (float)r.width;
	//		if (scale<3.5&&scale>2) {
	//			rectangle(mask, r, Scalar(0, 255, 0), 1);
	//			cout << "Width:" << r.width << "| Height:" << r.height << "\n";
	//			chars.push_back(bienso(r));
	//		}
	//	}*/
	//}
	//imshow("Contour", mask);
	//cout << "Chars size:" << chars.size()<<"\n";
	//for (int i = 0; i < chars.size(); i++) {
	//	char name[50];
	//	sprintf_s(name, "name %d", i);
	//	imshow(name, chars[i]);
	//}
}



int main() {
	Mat img = imread("image1.png");
	//imshow("input", img);
	process(img);
	waitKey(0);
}