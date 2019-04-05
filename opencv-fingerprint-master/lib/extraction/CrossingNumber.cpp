#include "CrossingNumber.h"
#include "Minutiae.h"

#include <opencv2/core/core.hpp>
#include <iostream>

using namespace cv;

namespace crossingNumber {

void getMinutiae(Mat& im, std::vector<Minutiae>& minutiae, int border ){
    if(border < 1){
        border = 2;
    }

    int bifurcationCount = 0;
    for(int i = border; i<=(im.cols-border); i++){
        for(int j = border; j<=(im.rows-border); j++){
            int cn = 0;

            //black pixel found?
            if(im.at<uchar>(cv::Point(i, j)) == 0){
                //iterate through 8-Neighborhood and count all transitions from black to white and reverse
                cn = cn + abs(im.at<uchar>(cv::Point(i-1, j-1))/255 - im.at<uchar>(cv::Point(i  , j-1))/255);
                cn = cn + abs(im.at<uchar>(cv::Point(i  , j-1))/255 - im.at<uchar>(cv::Point(i+1, j-1))/255);
                cn = cn + abs(im.at<uchar>(cv::Point(i+1, j-1))/255 - im.at<uchar>(cv::Point(i+1, j  ))/255);
                cn = cn + abs(im.at<uchar>(cv::Point(i+1, j  ))/255 - im.at<uchar>(cv::Point(i+1, j+1))/255);
                cn = cn + abs(im.at<uchar>(cv::Point(i+1, j+1))/255 - im.at<uchar>(cv::Point(i  , j+1))/255);
                cn = cn + abs(im.at<uchar>(cv::Point(i  , j+1))/255 - im.at<uchar>(cv::Point(i-1, j+1))/255);
                cn = cn + abs(im.at<uchar>(cv::Point(i-1, j+1))/255 - im.at<uchar>(cv::Point(i-1, j  ))/255);
                cn = cn + abs(im.at<uchar>(cv::Point(i-1, j  ))/255 - im.at<uchar>(cv::Point(i-1, j-1))/255);
                cn = cn/2;
                //std::cout<<"cn = " << cn << std::endl;

                if(cn == 3){
                    Minutiae minut(i, j, Minutiae::Type::BIFURCATION);
                    minutiae.push_back(minut);
                    bifurcationCount++;
                }
            }
        }
    }

    std::cout<<"Bifurcations count: " << bifurcationCount << std::endl;
}





}
