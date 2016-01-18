#ifndef SMOOTH_GAIN_DEVICE_H
#define SMOOTH_GAIN_DEVICE_H

#include <opencv2/opencv.hpp>

#include <SingleInputDevice.hpp>

class SmoothGainDevice : virtual public SingleInputDevice<cv::Point2f, cv::Point2f> {

    private:

        // the impulse gain parameters
        float a, b, c, d, e;

        // the base class buffer
        CircularBuffer<cv::Point2f> *buffer;

        // the output value
        cv::Point2f output;

    public:

        // basic constructor
        SmoothGainDevice(float a_, float b_, float c_, float d_, float e_) :
            SingleInputDevice<cv::Point2f, cv::Point2f>::SingleInputDevice(cv::Point2f(0.0, 0.0)),
            a(a_), b(b_), c(c_), d(d_), e(e_)
        {

            // get the buffer address
            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();

        }

        // the main method
        virtual void run() {

            // update the output
            output = buffer->pop();

            // update the horizontal
            if (0 > output.x) {

                if (d > output.x && e < output.x) {

                    // update the output
                    output.x = a*log(b*output.x + 1);

                } else if (e > output.x) {

                    // update the output
                    output.x = (c*output.x*output.x)*a*log(b*output.x + 1);

                } else {

                    // update the output
                    output.x = 0.0;

                }

            } else if (0 < output.x) {

                if (-d < output.x && -e > output.x) {

                    // update the output
                    output.x = -a*log(b*output.x + 1);

                } else if (-e < output.x) {

                    // update the output
                    output.x = -(c*output.x*output.x)*a*log(b*output.x + 1);

                } else {

                    // update the output
                    output.x = 0.0;

                }

            }

            // update the vertical
            if (0 > output.y) {

                if (d > output.y && e < output.y) {

                    // update the output
                    output.y = a*log(b*output.y + 1);

                } else if (e > output.y) {

                    // update the output
                    output.y = (c*output.y*output.y)*a*log(b*output.y + 1);

                } else {

                    // update the output
                    output.y = 0.0;

                }

            } else if (0 < output.y) {

                if (-d < output.y && -e > output.y) {

                    // update the output
                    output.y = -a*log(b*output.y + 1);

                } else if (-e < output.y) {

                    // update the output
                    output.y = -(c*output.y*output.y)*a*log(b*output.y + 1);

                } else {

                    // update the output
                    output.y = 0.0;

                }

            }

            // send the output value to external devices
            DeviceOutput<cv::Point2f>::send(output);

        }

};

#endif
