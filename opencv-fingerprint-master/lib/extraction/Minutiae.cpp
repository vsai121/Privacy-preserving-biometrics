#include "Minutiae.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"
using namespace cv;

Minutiae::Minutiae(int x, int y, Type t)
         : locX(x), locY(y), type(t)
{
    Minutiae::markedForErasing = false;
}

Minutiae::~Minutiae()
{
    //dtor
}

int Minutiae::getLocX()
{
    return locX;
}

int Minutiae::getLocY()
{
    return locY;
}

Minutiae::Type Minutiae::getType()
{
    return type;
}

void Minutiae::setMarkTrue()
{
    Minutiae::markedForErasing = true;
}

bool Minutiae::getMark()
{
    return Minutiae::markedForErasing;
}

double euclideanDistance(int x1, int y1, int x2, int y2){
    return sqrt(((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2)));
}


std::vector<int> Minutiae::findReference(Mat& im, std::vector<Minutiae>& minutiae, int border){

    int centreX = (im.rows - 2*border)/2;
    int centreY = (im.cols - 2*border)/2;
    std::vector<int> minut;

    double minDistance = 100000;

    for(int i = 0; i<minutiae.size(); i++){
        if(minutiae[i].getType() == 0){

          double distance = euclideanDistance(minutiae[i].getLocX(), minutiae[i].getLocY(), centreX, centreY);


          if(distance < 50 && distance < minDistance){
              minut.clear();
              minut.push_back(minutiae[i].getLocX());
              minut.push_back(minutiae[i].getLocY());
              minut.push_back(0);
              minDistance = distance;
          }

    }

}

  return minut;
}
