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

        // the main method
        virtual void run() {

            // update the output
            new_value = buffer->pop();
            if (new_value != new_value || std::isinf(new_value.x) || std::isinf(new_value.y)) {
                std::cout << std::endl << "Smoot recebeu coisa errada: " << new_value << std::endl;
                assert(false);
            }

            log_input.x = b*new_value.x + 1;
            log_input.y = b*new_value.y + 1;

            // update the horizontal
            if (0 < log_input.x) {

                if (d > new_value.x && e < new_value.x) {

                    // update the output
                    output.x = a*log(log_input.x);
                    // std::cout << std::endl << "0 < input.x && e < new_value.x SMOOTH New value and output: " << new_value << ", " << output << std::endl;

                } else if (e > new_value.x) {

                    // update the output
                    output.x = (c*new_value.x*new_value.x)*a*log(log_input.x);
                    // std::cout << std::endl << "0 < input.x && e > new value.x : SMOOTH New value and output: " << new_value << ", " << output << std::endl;

                } else {

                    // update the output
                    //output.x = new_value.x;
                    output.x = 0.0;

                }

            } else if (0 > log_input.x) {

                if (-d < new_value.x && -e > new_value.x) {

                    // update the output
                    output.x = -a*log(-log_input.x);
                    // std::cout << std::endl << "0 > input.x && -e > new_value.x: SMOOTH New value and output: " << new_value << ", " << output << std::endl;


                } else if (-e < new_value.x) {

                    // update the output
                    output.x = -(c*new_value.x*new_value.x)*a*log(-log_input.x);
                    // std::cout << std::endl << "0 > input.x && -e < new_value.x: SMOOTH New value and output: " << new_value << ", " << output << std::endl;

                } else {

                    // update the output
                    // output.x = new_value.x;
                    output.x = 0;

                }

            }

            // update the vertical
            if (0 < log_input.y) {

                if (d > new_value.y && e < new_value.y) {

                    // update the output
                    output.y = a*log(log_input.y);
                    // std::cout << std::endl << "0 < input.y && e < new_value.y: SMOOTH New value and output: " << new_value << ", " << output << std::endl;

                } else if (e > new_value.y) {

                    // update the output
                    output.y = (c*new_value.y*new_value.y)*a*log(log_input.y);
                    // std::cout << std::endl << "0 < input.y && e > new_value.y: SMOOTH New value and output: " << new_value << ", " << output << std::endl;

                } else {

                    // update the output
                    // output.y = new_value.y;
                    output.y = 0;

                }

            } else if (0 > log_input.y) {

                if (-d < new_value.y && -e > new_value.y) {

                    // update the output
                    output.y = -a*log(-log_input.y);
                    // std::cout << std::endl << "0 > input.y && -e > new_value.y:SMOOTH New value and output: " << new_value << ", " << output << std::endl;

                } else if (-e < new_value.y) {

                    // update the output
                    output.y = -(c*new_value.y*new_value.y)*a*log(-log_input.y);
                    // std::cout << std::endl << "0 > input.y && -e > new_value.y:SMOOTH New value and output: " << new_value << ", " << output << std::endl;

                } else {

                    // update the output
                    // output.y = new_value.y;
                    output.y = 0;

                }

            }

            if (output != output || std::isinf(output.x) || std::isinf(output.y)) {

                std::cout << std::endl << "Smooth gain produziu coisa errada: " << output << std::endl;
                assert(false);

            }

            // send the output value to external devices
            DeviceOutput<cv::Point2f>::send(output);

        }

};

#endif
