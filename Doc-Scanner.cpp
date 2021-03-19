#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include<iostream>

using namespace std;
using namespace cv;

//VideoCapture cap(0);
Mat imgOriginal, imgGray,imgBlur,imgThre, imgCanny, imgDia, finalWarpedDoc, doc_matrix, imgCroped;
vector<Point> initialPoints,docFinalPoints;

float w = 420, h = 596;//dimensions of A4 sheet

//Function to find edges of the documents and convert them into canny image
/////////////////////////---------Preprocessing-------------//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Mat preProcesssing(Mat img) {
	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
	Canny(imgGray, imgCanny, 25, 75);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(1, 1));
	dilate(imgCanny, imgDia, kernel);
	return imgDia;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------------Function to get contours----------------------------------
vector<Point> getContours(Mat img) {
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(img, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	vector<Point> biggestArea;
	int maxArea=0;

	for (int i=0; i < contours.size(); i++) {
		int area = contourArea(contours[i]);
		//cout << area;

		string objectType;
		float peri = arcLength(contours[i], true);
		approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
		if (area > maxArea && conPoly[i].size()==4) {
			maxArea = area;
			biggestArea = { conPoly[i][0],conPoly[i][1],conPoly[i][2],conPoly[i][3] };
			//drawContours(imgOriginal, conPoly, i, Scalar(255, 0, 255), 2);//to draw contours of maximam area
		}
	}
	return biggestArea;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////
//--------function to draw the points on image which is random----------------------
void drawPoints(vector<Point> randomPoints, Scalar color) {
	for (int i=0; i < randomPoints.size(); i++) {
		circle(imgOriginal, randomPoints[i],7, color, FILLED);
		putText(imgOriginal, to_string(i), randomPoints[i], FONT_HERSHEY_PLAIN, 2, color, 3);
	}
}
//----------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////
///---------function to convert the random points always in order of 0,1,2,3--------
//
//  at this point addint (x+y) == maximum .______.at this point subtracting (x-y) == maximum
//   let say (x,y)=(23,45)                |      |  let say (x,y)=(400,30)
//                                        |      |
//                                        |      |
//  at this point subtracting (x-y)== min !______!at this point adding (x+y) == minimum
//   let say(x,y) = (350,400)                       let say(x,y) = (25,300)
// to do the above shown idea
vector<Point> reorder(vector<Point> randomPoints) {
	vector<Point> newFinalPoints;
	
	//vector to store the sum of all (x,y) point (sumPoints) , vector to store the subtraction of all (x,y) value inside subPoints
	vector<int> sumPoints, subPoints;
	for (int i = 0; i < 4; i++) {
		sumPoints.push_back(randomPoints[i].x + randomPoints[i].y);//to store all addition of all randomPoints
		subPoints.push_back(randomPoints[i].x - randomPoints[i].y);//to store all substraction of all randomPoints
	}
	newFinalPoints.push_back(randomPoints[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);//it will give us the index valur of min element present inside sumPoint vector (this value should go at index value 0)
	newFinalPoints.push_back(randomPoints[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);//it will give us the index valur of max element present inside subPoint vector (this value should go at index value 1)
	newFinalPoints.push_back(randomPoints[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);//it will give us the index valur of min element present inside subPoint vector (this value should go at index value 2)
	newFinalPoints.push_back(randomPoints[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);// it will give us the index value of max element presen inside sumPoint vector (this value should go at index value 3)
    
	return newFinalPoints;
}
//-----------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// -------------------------------- Funtion to return and warp perspective image OR to return scanned document as top view perspective --------------------------------------------------------------------------------------
Mat getWarp(Mat imgOriginal, vector<Point> docFinalPoints, int width, int height) {
	
	////////////source points(docFinalPoints)
	//Point2f doc_src_finalPoints[4] = { {docFinalPoints[0].x,docFinalPoints[0].y},{docFinalPoints[1].x,docFinalPoints[0].y},{docFinalPoints[2].x,docFinalPoints[2].y},{docFinalPoints[3].x,docFinalPoints[3].y} };
	/*                                  OR             */
	Point2f doc_src_finalPoints[4] = { docFinalPoints[0],docFinalPoints[1],docFinalPoints[2],docFinalPoints[3] };
	
	////////////destination points
	Point2f doc_dst_finalPoints[4] = { {0.0f,0.0f},{w,0.0f},{0.0f,h},{w,h} };

	//////////// We have to user transformation Matrix ////////
	doc_matrix = getPerspectiveTransform(doc_src_finalPoints, doc_dst_finalPoints);///this matrix take some values and gives some correspending value after transformation

	//Warp Perspective
	warpPerspective(imgOriginal, finalWarpedDoc, doc_matrix, Point(w, h));

	return finalWarpedDoc;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



void main() {
	string path = "Resources/paper.jpg";
	imgOriginal = imread(path);

	/////////-resizing the imgOrignal
	//resize(imgOriginal, imgOriginal, Size(), 0.5, 0.5);
	
	/////////Preprocessing of the image
	imgThre = preProcesssing(imgOriginal);

	/////////Geting Contours of Images
	initialPoints = getContours(imgThre);

	/////////to draw intialpoint which is quite random in order every time 
	/*drawPoints(initialPoints, Scalar(0,0,255));*/

	/////////to make the initalpoint always in order 
	docFinalPoints = reorder(initialPoints);

	///////// to draw newFinalPoints which are now in order
	/*drawPoints(docFinalPoints, Scalar(0, 255, 0));*/

	/////////---- Warp Perspective
	finalWarpedDoc = getWarp(imgOriginal, docFinalPoints, w, h);

	////////-----Croping finalWarpedDoc to make it perfect	
	int cropValue = 5;//5 is no. of pixel we want to crop
	Rect roi(cropValue, cropValue, w - (2 * cropValue), h - (2 * cropValue));
	imgCroped = finalWarpedDoc(roi);


	imshow("Image", imgOriginal);
	//imshow("Image Threshold", imgThre);
	//imgshow("Image Warp",finalWarpedDoc);
	imshow("Scanned Document", imgCroped);
	waitKey(0);
}