#ifndef OPTICAL_FLOW_CPU_H
#define OPTICAL_FLOW_CPU_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

enum OpticalFlow {OPTICAL_FLOW_FARNEBACK_CPU, OPTICAL_FLOW_LUKAS_KANADE_PYR_CPU};

class OpticalFlowCPU {

    private:

        // the optical flow type
        OpticalFlow flow_type;

        // the optical flow components
        // just to avoid a lot of allocs
        cv::Mat flow, cflow;
        cv::UMat gray, old_gray, uflow;

        // the optical flow output
        cv::Point2f mean;

        // OPTICAL_FLOW_LUKAS_KANADE_PYR
        cv::TermCriteria termcrit;
        cv::Size subPixWinSize, winSize;

        const int MAX_COUNT;
        bool needToInit;

        // the good point to track
        std::vector<cv::Point2f> points[2];

        // the displacement between the points
        cv::Point2f displacement;

        std::vector<uchar> status;
        std::vector<float> err;

        // draw the points
        cv::Mat image;

    public:

        // basic constructor
        OpticalFlowCPU(OpticalFlow);

        // computes the optical flow
        cv::Point2f run(cv::Mat frame);

};

#endif
