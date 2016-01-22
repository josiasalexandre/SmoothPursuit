#ifndef IMAGE_MOTION_MODEL_H
#define IMAGE_MOTION_MODEL_H

#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <OpticalFlowCPU.hpp>
#include <StupidSlip.hpp>

#include <DSPSystem.hpp>


class ImageMotionModel {

    private:

        // the video capture device
        cv::VideoCapture video;

        // the frame rate and the sampling frequency
        float fps, fs;

        // the frame input
        cv::Mat frame;

        // the ROI
        cv::Rect fovea;

        // the fovea translation
        cv::Point2f translation;

        // the optical flow object
        OpticalFlowCPU optical_flow;

        // the stupid slip
        StupidSlip stupid;

        // private methods
        void mouse_click_callback(int, int, int, int);
        static void mouse_click(int, int, int, int, void*);
        void select_roi();
        void find_roi(cv::Mat);

        // the interpolated signal
        std::vector<cv::Point2f> interpolated, output;
        cv::Point2f last_flow, expected_mean, current_flow;
        bool interp;
        cv::Point2f displacement, last_displacement;

        // the DSP system
        DSPSystem system;

        // interpolation
        void linear_interpolation();

        unsigned int wait_time;
        float dt;

    public:

        // basic constructor with video filename and frame rate
        ImageMotionModel(const std::string, float);

        // basic constructor with camera and frame rate
        ImageMotionModel(float);

        // the main method
        void run();

};

#endif
