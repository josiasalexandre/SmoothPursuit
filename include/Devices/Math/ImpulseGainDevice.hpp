#ifndef IMPULSE_GAIN_DEVICE_H
#define IMPULSE_GAIN_DEVICE_H

#include <opencv2/opencv.hpp>

#include <SingleInputDevice.hpp>

class ImpulseGainDevice : virtual public SingleInputDevice<cv::Point2f, cv::Point2f> {

    private:

        // the impulse gain parameters
        float a, b, c;

        // the base class buffer
        CircularBuffer<cv::Point2f> *buffer;

        // the output value
        cv::Point2f output;

    public:

        // basic constructor
        ImpulseGainDevice(float a_, float b_, float c_) :
            SingleInputDevice<cv::Point2f, cv::Point2f>::SingleInputDevice(cv::Point2f(0.0, 0.0)),
            a(a_), b(b_), c(c_)
        {

            // get the buffer address
            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();

        }

        // the main method
        virtual void run() {

            // update the output
            output = buffer->pop();

            // update the horizontal
            if (c < output.x) {

                // update the output
                output.x = a*log(b*output.x + 1);

            } else if (-c > output.x) {

                // update the output
                output.x = -a*log(b*output.x + 1);

            } else {

                // update the output
                output.x = 0.0;

            }

            // update the vertical
            if (c < output.y) {

                // update the output
                output.y = a*log(b*output.y + 1);

            } else if (-c < output.x) {

                // update the output
                output.y = -a*log(b*output.y + 1);

            } else {

                // update the output
                output.y = 0.0;

            }

            // send the output value to external devices
            DeviceOutput<cv::Point2f>::send(output);

        }

};

#endif
