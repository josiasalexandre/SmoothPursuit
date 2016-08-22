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

        cv::Point2f new_value, log_input;

    public:

        // basic constructor
        SmoothGainDevice(float a_, float b_, float c_, float d_, float e_) :
            SingleInputDevice<cv::Point2f, cv::Point2f>::SingleInputDevice(cv::Point2f(0.0, 0.0)),
            a(a_), b(b_), c(c_), d(d_), e(e_)
        {

            // get the buffer address
            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();

        }

        // device reset
        virtual void reset() {

            // set the output to zero
            output.x = 0.0;
            output.y = 0.0;

            // clear the entire input buffer
            buffer->clear();

        }

        // the main method
        virtual void run() {

            // update the output
            new_value = buffer->pop();

            // update the horizontal
            if (0 <= new_value.x) {

                log_input.x = b*new_value.x + 1;

                if (d > new_value.x && e < new_value.x) {

                    // update the output
                    output.x = a*log(log_input.x);

                } else if (e > new_value.x) {

                    // update the output
                    output.x = (c*new_value.x*new_value.x)*a*log(log_input.x);

                } else {

                    // update the output
                    output.x = 0.0;

                }

            } else if (0 > new_value.x) {

                if (-d < new_value.x && -e > new_value.x) {

                    // update the output
                    output.x = -a*log(b*(-new_value.x) + 1);

                } else if (-e < new_value.x) {

                    // update the output
                    output.x = -(c*new_value.x*new_value.x)*a*log(b*(-new_value.x) + 1);

                } else {

                    // update the output
                    output.x = 0;

                }

            }

            // update the vertical
            if (0 <= new_value.y) {

                log_input.y = b*new_value.y + 1;

                if (d > new_value.y && e < new_value.y) {

                    // update the output
                    output.y = a*log(log_input.y);

                } else if (e > new_value.y) {

                    // update the output
                    output.y = (c*new_value.y*new_value.y)*a*log(log_input.y);

                } else {

                    // update the output
                    output.y = 0;

                }

            } else if (0 > new_value.y) {

                if (-d < new_value.y && -e > new_value.y) {

                    // update the output
                    output.y = -a*log(b*(-new_value.y) + 1);

                } else if (-e < new_value.y) {

                    // update the output
                    output.y = -(c*new_value.y*new_value.y)*a*log(b*(-new_value.y) + 1);

                } else {

                    // update the output
                    output.y = 0;

                }

            }

            // send the output value to external devices
            DeviceOutput<cv::Point2f>::send(output);

        }

};

#endif
