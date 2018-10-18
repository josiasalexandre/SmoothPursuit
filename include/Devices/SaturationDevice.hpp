#ifndef SATURATION_DEVICE_H
#define SATURATION_DEVICE_H

#include <opencv2/opencv.hpp>

#include <SingleInputDevice.hpp>

class SaturationDevice : virtual public SingleInputDevice<cv::Point2f, cv::Point2f>
{
    private:

        CircularBuffer<cv::Point2f> *buffer;

        cv::Point2f output;

        float saturation;

    public:

        // basic constructor
        SaturationDevice(float value) :
            SingleInputDevice<cv::Point2f, cv::Point2f>::SingleInputDevice(cv::Point2f(0.0, 0.0)), saturation(value), buffer(nullptr)
        {
            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();
            if (nullptr == buffer) { throw std::bad_alloc(); }
        }

        // overriding the main method
        virtual void run()
        {
            output = buffer->pop();

            if (saturation < output.x)
            {
                output.x = saturation;
            }
            else if (-saturation > output.x)
            {
                output.x = -saturation;
            }


            if (saturation < output.y)
            {
                output.y = saturation;
            }
            else if (-saturation > output.y)
            {
                output.y = -saturation;
            }

            // send the output to external devics
            DeviceOutput<cv::Point2f>::send(output);
        }
};

#endif
