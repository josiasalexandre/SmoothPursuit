#ifndef OPTICAL_FLOW_GPU_DEVICE_H
#define OPTICAL_FLOW_GPU_DEVICE_H

#include <vector>

#include <opencv2/core/utility.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgcodecs.hpp>

#include <SingleInputDevice.hpp>

class OpticalFlowGpuDevice : virtual public SingleInputDevice<cv::Mat3b, std::vector<cv::Point2f>> {

    public:

    // basic constructor
    OpticalFlowGpuDevice(cv::Mat3b v_null);

};

#endif
