#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"
#include <iostream>
#include <fstream>
#include <math.h>
#include<bitset>

#include "AdaptiveLocalThreshold.h"
#include "Ideka.h"
#include "GuoHall.h"
#include "CrossingNumber.h"
#include "Filter.h"
#include "ZhangSuen.h"
#include "Minutiae.h"



using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;






Mat binarise(Mat input){

	Mat img = input.clone();

  localThreshold::binarisation(img, int(input.cols/10), int(input.rows/10));
  cv::threshold(img, img, 50, 255, cv::THRESH_BINARY);
  Mat binImg = img.clone();
  ideka::binOptimisation(img);

	return img;

}


Mat thin (Mat img){

	cv::bitwise_not(img, img);    //Inverse for bit-operations
  GuoHall::thinning(img);
  cv::bitwise_not(img, img);

	return img;
}



vector<KeyPoint> computeKeypoints(Mat img){



  vector<Minutiae> minutiae;
  crossingNumber::getMinutiae(img, minutiae, 40);
  //cout<<"Minutaie size: " << minutiae.size() << endl;



  Mat minutImg = img.clone();
  cvtColor(img, minutImg, cv::COLOR_GRAY2RGB);

  for(std::vector<Minutiae>::size_type i = 0; i<minutiae.size(); i++){
      int squareSize = 3;
      Mat roi = minutImg(Rect(minutiae[i].getLocX()-squareSize/2, minutiae[i].getLocY()-squareSize/2, squareSize, squareSize));
      double alpha = 0.3;


      if(minutiae[i].getType() == Minutiae::Type::RIDGEENDING){
          Mat color(roi.size(), CV_8UC3, cv::Scalar(255,0,0));
          addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
      }else if(minutiae[i].getType() == Minutiae::Type::BIFURCATION){
          Mat color(roi.size(), CV_8UC3, cv::Scalar(0,0,255));
          addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
      }

  }


  //imshow( "Minutaie", minutImg );
  //waitKey(0);

  Filter::filterMinutiae(minutiae);
  //cout<<"Performing filtering: " << minutiae.size() << endl;

  Mat minutImg2 = img.clone();
  cvtColor(img, minutImg2, cv::COLOR_GRAY2RGB);
  for(std::vector<Minutiae>::size_type i = 0; i<minutiae.size(); i++){

      int squareSize = 5;     //has to be uneven
      Mat roi = minutImg2(Rect(minutiae[i].getLocX()-squareSize/2, minutiae[i].getLocY()-squareSize/2, squareSize, squareSize));  Mat orientImg;
      double alpha = 0.3;
      if(minutiae[i].getType() == Minutiae::Type::RIDGEENDING){
          Mat color(roi.size(), CV_8UC3, cv::Scalar(255,0,0));
          addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
      }else if(minutiae[i].getType() == Minutiae::Type::BIFURCATION){
          Mat color(roi.size(), CV_8UC3, cv::Scalar(0,0,255));
          addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
      }

  }



  //imshow( "Minutiae after filtering", minutImg2 );
  //waitKey(0);


  vector<KeyPoint> keypoints;

  for(std::vector<Minutiae>::size_type i = 0; i<minutiae.size(); i++){

    float x = minutiae[i].getLocX();
    float y = minutiae[i].getLocY();

    Point2f a(x,y);



    KeyPoint k(a,1,-1,0,0,minutiae[i].getType());

    keypoints.push_back(k);

  }

	return keypoints;

}


Mat computeDescriptors(Mat img, vector<KeyPoint> keypoints){


  Ptr<Feature2D> orb_descriptor = ORB::create();
  Mat descriptors;
  orb_descriptor->compute(img, keypoints, descriptors);

  //cout<<"descriptors\n";

  //cout<<descriptors.rows<<"   "<<descriptors.cols<<endl;


  vector<vector<int>> matrix(descriptors.rows, vector<int>(descriptors.cols));

  for(int i = 0; i < descriptors.rows; i++) {
    std::string binary_string = "";
    for(int j = 0; j < descriptors.cols; j++) {
      binary_string += bitset< 8 >( descriptors.at<uchar>(i,j) ).to_string();
      matrix[i][j] = (bitset< 8 >( descriptors.at<uchar>(i,j) ).to_ulong());
      ////cout<<matrix[i][j]<<"  ";

    }

    ////cout<<endl;

  }

  fstream featureFile("feature.txt", fstream::out);
  featureFile<<"[ "<<descriptors.rows<<" ]"<<endl;

  for(int i = 0 ; i< descriptors.rows ; i++){
    featureFile<<"[ ";
    for(int j = 0 ; j<descriptors.cols ; j++){
      ////cout<<matrix[i][j]<<"  ";
      featureFile<< matrix[i][j]<<" ";

    }
    ////cout<<endl;
    featureFile<<"]";
    featureFile<<endl;
  }

  featureFile.close();

  return descriptors;

}


int main( int argc, const char* argv[] )
{

	Mat input1 = imread(argv[1], IMREAD_GRAYSCALE);
	Mat input2 = imread(argv[2], IMREAD_GRAYSCALE);

	Mat binarisedImg1 = binarise(input1);
	//imshow( "Minutaie", binarisedImg1 );
  //waitKey(0);
	Mat binarisedImg2 = binarise(input2);
	//imshow( "Minutaie", binarisedImg2 );
	//waitKey(0);


	Mat thinnedImg1 = thin(binarisedImg1);
	//imshow( "Minutaie", thinnedImg1 );
  //waitKey(0);
	Mat thinnedImg2 = thin(binarisedImg2);
	//imshow( "Minutaie", thinnedImg2 );
  //waitKey(0);


  vector<KeyPoint> keypoints1 = computeKeypoints(thinnedImg1);
  vector<KeyPoint> keypoints2 = computeKeypoints(thinnedImg2);

	Mat descriptors1 = computeDescriptors(binarisedImg1, keypoints1);
	Mat descriptors2 = computeDescriptors(binarisedImg2, keypoints2);

  Ptr<BFMatcher> matcher = BFMatcher::create(NORM_HAMMING, false);
  vector< DMatch > matches;
  matcher->match(descriptors1, descriptors2, matches);

    float score = 0.0;
    for(int i=0; i < matches.size(); i++){
        DMatch current_match = matches[i];
        score = score + current_match.distance;
    }
    cerr << endl << "Current matching score = " << score/(descriptors1.rows) << endl;


}
