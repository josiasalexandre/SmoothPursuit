#ifndef IMAGE_MOTION_MODEL_H
#define IMAGE_MOTION_MODEL_H

#include <InputVideoDevice.hpp>
#include <FoveaDevice.hpp>
#include <OpticalFlowCPUDevice.hpp>
#include <VelocityInterpolationDevice.hpp>
#include <DSPSystem.hpp>

class ImageMotionModel {

    private:

        // video capture device
        InputVideoDevice video;

        // fovea device
        FoveaDevice fovea;

        // the optical flow device
        OpticalFlowCPUDevice optical_flow;

        // velocity interpolation device
        VelocityInterpolationDevice interpolation;

        cv::Point2f translation;
        // the smooth pursuit model
        //DSPSystem system;

    public:

        // basic constructor with video filename and frame rate
        ImageMotionModel(std::string, float);

        // basic constructor with camera and frame rate
        ImageMotionModel(float);

        //
        void run() {}
};

#endif
