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



int main( int argc, const char* argv[] )
{

  Mat input = imread("../../data/103_1.tif", IMREAD_GRAYSCALE);

  if(input.empty()){
    cerr << "Image not read correctly. Check if path is correct!" << endl;
  }

  Mat img = input.clone();
  localThreshold::binarisation(img, 41, 56);
  cv::threshold(img, img, 50, 255, cv::THRESH_BINARY);
  Mat binImg = img.clone();
  ideka::binOptimisation(img);



  cv::bitwise_not(img, img);
  GuoHall::thinning(img);
  cv::bitwise_not(img, img);


  vector<Minutiae> minutiae;
  crossingNumber::getMinutiae(img, minutiae, 20);
  cout<<"Minutaie size: " << minutiae.size() << endl;




  //Visualisation
  Mat minutImg = img.clone();
  cvtColor(img, minutImg, CV_GRAY2RGB);

  for(std::vector<Minutiae>::size_type i = 0; i<minutiae.size(); i++){
      int squareSize = 3;
      Mat roi = minutImg(Rect(minutiae[i].getLocX()-squareSize/2, minutiae[i].getLocY()-squareSize/2, squareSize, squareSize));
      double alpha = 0.3;

      if(minutiae[i].getType() == Minutiae::Type::BIFURCATION){
          Mat color(roi.size(), CV_8UC3, cv::Scalar(0,0,255));
          addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
      }

  }


  ////imshow( "Minutaie", minutImg );
  //waitKey(0);
  Filter::filterMinutiae(minutiae);

  Mat minutImg2 = img.clone();
  cvtColor(img, minutImg2, CV_GRAY2RGB);
  for(std::vector<Minutiae>::size_type i = 0; i<minutiae.size(); i++){

      int squareSize = 5;     //has to be uneven
      Mat roi = minutImg2(Rect(minutiae[i].getLocX()-squareSize/2, minutiae[i].getLocY()-squareSize/2, squareSize, squareSize));
      double alpha = 0.3;
      if(minutiae[i].getType() == Minutiae::Type::BIFURCATION){
          Mat color(roi.size(), CV_8UC3, cv::Scalar(0,0,255));
          addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
      }

  }



  imshow( "Minutiae after filtering", minutImg2 );
  waitKey(0);


  vector<vector <int>> dist(minutiae.size(), vector<int>(minutiae.size(),0));

  for(int i = 0 ; i<minutiae.size() ; i++){
      for(int j = i+1; j<minutiae.size() ; j++){
          dist[i][j] = Filter::euclideanDistance(minutiae[i].getLocX(), minutiae[i].getLocY(), minutiae[j].getLocX(), minutiae[j].getLocY());
          dist[j][i] = dist[i][j];
      }
  }


  for(int i = 0 ; i<dist.size() ; i++){
    for(int j = 0 ; j<dist[0].size(); j++){
      cout<<dist[i][j]<<"  ";

    }
    sort(dist[i].begin(), dist[i].end());
    cout<<endl;
  }

  fstream featureFile("../FeatureData/feature103_1.txt", fstream::out);
  featureFile<<"[ "<<dist.size()<<" ]"<<endl;


  for(int i = 0 ; i<dist.size() ; i++){
    featureFile<<"[ ";
    for(int j = 0 ; j<dist[0].size(); j++){
      cout<<dist[i][j]<<"  ";
      featureFile<<dist[i][j]<<" ";

    }

    cout<<endl;
    featureFile<<" ]";
    featureFile<<endl;

  }

  featureFile.close();







  //waitKey(0);









}
