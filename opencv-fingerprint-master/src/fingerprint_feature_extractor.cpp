#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"
#include <iostream>

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

typedef void (*algorithm_function_ptr_t)(cv::Mat&);

void originalImage(cv::Mat& source) {
    return;
}


cv::Mat skeletonize(cv::Mat source, int threshold) {
    algorithm_function_ptr_t algorithms[] = {GuoHall::thinning  };
    int num_algorithms = sizeof(algorithms)/sizeof(algorithms[0]);


    cv::Mat img = source.clone();
    cv::threshold(img, img, threshold, 255, cv::THRESH_BINARY);

    //cv::Mat temp(img.size(), CV_8UC1, cv::Scalar(0));
    cv::Mat output(img.size().height, img.size().width * num_algorithms,  CV_8UC1, cv::Scalar(0));

    for(int i=0; i<num_algorithms; i++) {
        cv::Mat temp = img.clone();
        algorithms[i](temp);

        cv::Mat targetArea = output(cv::Rect(img.size().width * i, 0, img.size().width, img.size().height));
        temp.copyTo(targetArea);
    }

    return output;
}

int trackbarValue = 127;
cv::Mat sourceImage;


void on_mouse(int event, int x, int y, int flags, void* param) {
    if (event != cv::EVENT_LBUTTONDOWN) {
        return;
    }
    std::cout << "Re-computing with threshold " << trackbarValue << std::endl;
    cv::Mat output = skeletonize(sourceImage, trackbarValue);
    imshow("Skeletenization", output); waitKey(0);

}

int main( int argc, const char* argv[] )
{

  cout<<"Hello world\n";
  Mat input = imread("../../data/image4.png", IMREAD_GRAYSCALE);

  if(input.empty()){
    cerr << "Image not read correctly. Check if path is correct!" << endl;
  }

  Mat input_binary;
  threshold(input, input_binary, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

  // Compare both
  Mat container(input.rows, input.cols*2, CV_8UC1);
  input.copyTo( container( Rect(0, 0, input.cols, input.rows) ) );
  input_binary.copyTo( container( Rect(input.cols, 0, input.cols, input.rows) ) );
  imshow("input versus binary", container); waitKey(0);

  cv::createTrackbar("Threshold", "Skeletenization", &trackbarValue, 255);
  cv::setMouseCallback("Skeletenization", on_mouse);

  // Thinning
  cv::Mat input_thinned = skeletonize(input_binary, trackbarValue);
  imshow("Skeletenization", input_thinned);

  cv::waitKey(0);

  vector<Minutiae> minutiae;
  crossingNumber::getMinutiae(input_thinned, minutiae, 20);
  cout<<"Minutaie size: " << minutiae.size() << endl;

  //Visualisation
  Mat minutImg = input_thinned.clone();
  cvtColor(input_thinned, minutImg, CV_GRAY2RGB);

  for(std::vector<Minutiae>::size_type i = 0; i<minutiae.size(); i++){
      //add an transparent square at each minutiae-location
      int squareSize = 3;     //has to be uneven
      Mat roi = minutImg(Rect(minutiae[i].getLocX()-squareSize/2, minutiae[i].getLocY()-squareSize/2, squareSize, squareSize));
      double alpha = 0.3;


      if(minutiae[i].getType() == Minutiae::Type::RIDGEENDING){
          Mat color(roi.size(), CV_8UC3, cv::Scalar(255,0,0));    //blue square for ridgeending
          addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
      }else if(minutiae[i].getType() == Minutiae::Type::BIFURCATION){
          Mat color(roi.size(), CV_8UC3, cv::Scalar(0,0,255));    //red square for bifurcation
          addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
      }

  }


  imshow( "Minutaie", minutImg );
  waitKey(0);

  Filter::filterMinutiae(minutiae);
  cout<<"Performing filtering: " << minutiae.size() << endl;

  Mat minutImg2 = input_thinned.clone();
  cvtColor(input_thinned, minutImg2, CV_GRAY2RGB);
  for(std::vector<Minutiae>::size_type i = 0; i<minutiae.size(); i++){
      //add an transparent square at each minutiae-location
      int squareSize = 5;     //has to be uneven
      Mat roi = minutImg2(Rect(minutiae[i].getLocX()-squareSize/2, minutiae[i].getLocY()-squareSize/2, squareSize, squareSize));
      double alpha = 0.3;
      if(minutiae[i].getType() == Minutiae::Type::RIDGEENDING){
          Mat color(roi.size(), CV_8UC3, cv::Scalar(255,0,0));    //blue square for ridgeending
          addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
      }else if(minutiae[i].getType() == Minutiae::Type::BIFURCATION){
          Mat color(roi.size(), CV_8UC3, cv::Scalar(0,0,255));    //red square for bifurcation
          addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
      }

  }
       // Create a window for display.
  imshow( "Minutiae after filtering", minutImg2 );                 // Show our image inside it.


  waitKey(0);                // Show our image inside it.







}
