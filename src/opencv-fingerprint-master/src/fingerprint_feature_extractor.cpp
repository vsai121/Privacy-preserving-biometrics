#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"
#include <iostream>
#include <fstream>
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


int main( int argc, const char* argv[] )
{

  cout<<"Hello world\n";
  Mat input = imread("../../data/img1.png", IMREAD_GRAYSCALE);

  if(input.empty()){
    cerr << "Image not read correctly. Check if path is correct!" << endl;
  }

  Mat img = input.clone();
  localThreshold::binarisation(img, 41, 56);
  cv::threshold(img, img, 50, 255, cv::THRESH_BINARY);
  Mat binImg = img.clone();
  ideka::binOptimisation(img);



  cv::bitwise_not(img, img);    //Inverse for bit-operations
  GuoHall::thinning(img);
  cv::bitwise_not(img, img);


  vector<Minutiae> minutiae;
  crossingNumber::getMinutiae(img, minutiae, 20);
  cout<<"Minutaie size: " << minutiae.size() << endl;

  //Visualisation
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


  imshow( "Minutaie", minutImg );
  waitKey(0);

  Filter::filterMinutiae(minutiae);
  cout<<"Performing filtering: " << minutiae.size() << endl;

  Mat minutImg2 = img.clone();
  cvtColor(img, minutImg2, cv::COLOR_GRAY2RGB);
  for(std::vector<Minutiae>::size_type i = 0; i<minutiae.size(); i++){

      int squareSize = 5;     //has to be uneven
      Mat roi = minutImg2(Rect(minutiae[i].getLocX()-squareSize/2, minutiae[i].getLocY()-squareSize/2, squareSize, squareSize));
      double alpha = 0.3;
      if(minutiae[i].getType() == Minutiae::Type::RIDGEENDING){
          Mat color(roi.size(), CV_8UC3, cv::Scalar(255,0,0));
          addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
      }else if(minutiae[i].getType() == Minutiae::Type::BIFURCATION){
          Mat color(roi.size(), CV_8UC3, cv::Scalar(0,0,255));
          addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
      }

  }

  imshow( "Minutiae after filtering", minutImg2 );
  waitKey(0);

  vector<KeyPoint> keypoints;

  for(std::vector<Minutiae>::size_type i = 0; i<minutiae.size(); i++){

      //cout<<minutiae[i].getLocX()<<"   "<<minutiae[i].getLocY()<<"     "<<minutiae[i].getType()<<"  "<<endl;
      keypoints.push_back(KeyPoint(minutiae[i].getLocX(), minutiae[i].getLocY(), minutiae[i].getType()));
  }


  Ptr<Feature2D> orb_descriptor = ORB::create();
  Mat descriptors;
  orb_descriptor->compute(img, keypoints, descriptors);

  vector<vector<int>> matrix(descriptors.rows, vector<int>(descriptors.cols));

  for(int i = 0; i < descriptors.rows; i++) {
    std::string binary_string = "";
    for(int j = 0; j < descriptors.cols; j++) {
      binary_string += bitset< 8 >( descriptors.at<uchar>(i,j) ).to_string();
      matrix[i][j] = (bitset< 8 >( descriptors.at<uchar>(i,j) ).to_ulong());
    }

    cout << i << ") " << binary_string <<endl;
  }

  fstream featureFile("../FeatureData/feature.txt", fstream::out);
  featureFile<<"[ "<<descriptors.rows<<" ]"<<endl;

  for(int i = 0 ; i< descriptors.rows ; i++){
    featureFile<<"[ ";
    for(int j = 0 ; j<descriptors.cols ; j++){
      cout<<matrix[i][j]<<"  ";
      featureFile<< matrix[i][j]<<" ";

    }
    cout<<endl;
    featureFile<<"]";
    featureFile<<endl;
  }

  featureFile.close();



  waitKey(0);







}
