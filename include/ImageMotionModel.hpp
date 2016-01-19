#ifndef IMAGE_MOTION_MODEL_H
#define IMAGE_MOTION_MODEL_H

#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <OpticalFlowCPU.hpp>

#include <DSPSystem.hpp>

class ImageMotionModel {

    private:

        // the video capture device
        cv::VideoCapture video;

        // the frame rate and the sampling frequency
        float fps, fs, inverse_fs;

        // the frame input
        cv::Mat frame;

        // the ROI
        cv::Rect fovea;

        // the fovea translation
        cv::Point2f translation;

        // the optical flow object
        OpticalFlowCPU optical_flow;

        // private methods
        void mouse_click_callback(int, int, int, int);
        static void mouse_click(int, int, int, int, void*);
        void select_roi();

        // the interpolated signal
        std::vector<cv::Point2f> interpolated;
        cv::Point2f old_mean, current_mean;
        bool interp;

        // the DSP system
        DSPSystem system;

        // interpolation
        void linear_interpolation();

    public:

        // basic constructor with video filename and frame rate
        ImageMotionModel(const std::string, float);

        // basic constructor with camera and frame rate
        ImageMotionModel(float);

        // the main method
        void run();

};

#endif
