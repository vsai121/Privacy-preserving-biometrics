#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"

#ifndef MINUTIAE_H
#define MINUTIAE_H



class Minutiae
{
    public:
        enum Type {BIFURCATION, RIDGEENDING};
        Minutiae(int locX, int locY, Type type);
        virtual ~Minutiae();
        int getLocX();
        int getLocY();
        Type getType();
        void setMarkTrue();
        bool getMark();
        static std::vector<int> findReference(cv::Mat& im, std::vector<Minutiae>& minutiae, int border);
    protected:
    private:
        int locX;
        int locY;
        Type type;
        bool markedForErasing;

};

#endif // MINUTIAE_H
