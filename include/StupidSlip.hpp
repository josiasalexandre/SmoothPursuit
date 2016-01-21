#ifndef STUPID_SLIP_H
#define STUPID_SLIP_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class StupidSlip {

    private:

        // the gray converted image
        cv::Mat gray;

        // the max value
        unsigned int threshold;

    public:

        // basic constructor
        StupidSlip(unsigned int);

        // calculates the displacement from the fovea's center
        cv::Point2i displacement(cv::Point2i, cv::Mat);


};

#endif
