#ifndef OPTICAL_FLOW_CPU_H
#define OPTICAL_FLOW_CPU_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

enum OpticalFlow {OPTICAL_FLOW_FARNEBACK_CPU, OPTICAL_FLOW_LUKAS_KANADE_PYR_CPU};

class OpticalFlowCPU
{
    private:

        // the optical flow output
        cv::Point2f mean;

        // the optical flow type
        OpticalFlow flow_type;

        // the optical flow components
        // just to avoid a lot of allocs
        cv::Mat flow, cflow;
        cv::UMat old_gray, uflow;

        // the movement directions
        cv::Point2f horizontal, vertical;

        // the object and background movement
        cv::Point2f object, background;

        // auxiliar directions vars
        int hl, hr, vd, vu;

        float min_flow;;

        // the gaussian kernel
        cv::Mat gaussian;

        // the indexes to convolution
        unsigned int k_w, k_h;

        // the gaussian pre-computed values
        float sigma, left_g, right_g;

        // private methods
        void build_gaussian_kernel(unsigned int);
        float gaussian_weight(cv::Point2i, cv::Point2i);

        // draw flow map
        void drawOptFlowMap(const cv::Mat&, cv::Mat&, int, double, const cv::Scalar&);

        // OPTICAL_FLOW_LUKAS_KANADE_PYR
        cv::TermCriteria termcrit;
        cv::Size subPixWinSize, winSize;

        const int MAX_COUNT;
        bool needToInit;

        // the good point to track
        std::vector<cv::Point2f> points[2];

        std::vector<uchar> status;
        std::vector<float> err;

        // the new clicked point
        cv::Point2f clicked_point;

        // add point flag
        bool addPoint;

        // the estimated point
        cv::Point2f estimated, mid_point;

        // the background threshold
        float bg_threshold, mo_threshold;

        // just to draw the circles
        cv::Mat colored;

        // private methods
        void get_points(cv::Mat);
        void mouse_click_callback(int, int, int, int);
        static void mouse_click(int, int, int, int, void*);

    public:

        // basic constructor
        OpticalFlowCPU(OpticalFlow);

        // computes the optical flow
        cv::Point2f run(cv::Mat);

        // computes the optical flow - LKPyr
        cv::Point2f run_lkpyr(cv::Mat);

        // computes the optical flow - LKPyr
        cv::Point2f run_lkpyr(cv::Mat, cv::Point2f);

        // computes the optical flow - LKPyr
        cv::Point2f run_lkpyr2(cv::Mat new_gray, cv::Point2f movement);

        // computes the optical flow - Farneback
        cv::Point2f run_farneback(cv::Mat);

        // computes the optical flow - Farneback overloaded
        cv::Point2f run_farneback(cv::Mat, cv::Point2i);

        // reset the flag
        void init(cv::Mat);
};

#endif
