#ifndef IMAGE_MOTION_MODEL_H
#define IMAGE_MOTION_MODEL_H

#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <OpticalFlowCPU.hpp>
#include <StupidSlip.hpp>

#include <DSPSystem.hpp>


class ImageMotionModel
{
    private:

        // the video capture device
        cv::VideoCapture input_video;

        // the output video
        cv::VideoWriter output_video;

        // the frame rate and the sampling frequency
        float fps, fs;

        // the frame input
        cv::Mat frame;

        // the ROI
        cv::Rect fovea;

        // the fovea translation
        cv::Point2f translation, old_translation;

        // the optical flow object
        OpticalFlowCPU optical_flow;

        // the stupid slip
        StupidSlip stupid;

        // for testing purpose
        bool testing;

        // private methods
        void MouseClickCallback(int, int, int, int);
        static void MouseClick(int, int, int, int, void*);
        void SelectRoi();
        void FindRoi(cv::Mat);

        // the interpolated signal
        std::vector<cv::Point2f> interpolated, output;
        cv::Point2f last_flow, expected_mean, current_flow;
        cv::Point2f displacement;

        unsigned int wait_time;
        float dt;

        // the groundtruth filename
        std::string groundtruth;

        // the groundtruth flag
        bool hasgroundtruth;

        // the DSP system
        DSPSystem system;

        // interpolation
        void LinearInterpolation();

        // verify if two bounding box overlaps with each other
        bool Overlap(const cv::Rect &a, const cv::Rect &b);

        // get the external frame
        cv::Rect GetExternalFrame(cv::Rect);

        // get the external frame
        cv::Rect GetInternalFrame(cv::Rect, cv::Rect);

        void GetFoveaDimension(cv::Rect&);

    public:

        // basic constructor with video filename and frame rate
        ImageMotionModel(const std::string input_video, float frame_rate, const std::string ground, std::string output_video);

        // basic constructor with camera and frame rate
        ImageMotionModel(float);

        // the main method
        void Run(const std::string&);

};

#endif
