#ifndef LINEAR_GAIN_DEVICE_H
#define LINEAR_GAIN_DEVICE_H

#include <opencv2/opencv.hpp>

#include <SingleInputDevice.hpp>

class LinearGainDevice : virtual public SingleInputDevice<cv::Point2f, cv::Point2f>
{
    private:

        // alpha multiplier
        float alpha;

        // the output value
        cv::Point2f output;

        // the base class buffer
        CircularBuffer<cv::Point2f> *buffer;

    public:

        // basic constructor
        LinearGainDevice(float a) :
            SingleInputDevice<cv::Point2f, cv::Point2f>::SingleInputDevice(cv::Point2f(0.0, 0.0)), alpha(a)
        {
            // get the buffer address
            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();
        }

        // the reset
        virtual void reset()
        {
            // set the output to zero
            output.x = 0.0;
            output.x = 0.0;

            // clear the buffer
            buffer->clear();
        }

        // the main method
        virtual void run()
        {
            // get the new value
            output = buffer->pop();

            output *= alpha;

            // send the output value to external devices
            DeviceOutput<cv::Point2f>::send(output);
        }
};

#endif
