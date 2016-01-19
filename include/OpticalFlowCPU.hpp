#ifndef OPTICAL_FLOW_CPU_H
#define OPTICAL_FLOW_CPU_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


class OpticalFlowCPU {

    private:

        // the optical flow components
        // just to avoid a lot of allocs
        cv::Mat flow, cflow;
        cv::UMat gray, old_gray, uflow;

        // the optical flow output
        cv::Point2f mean;

    public:

        // computes the optical flow
        cv::Point2f run(cv::Mat frame);

};

#endif
