#ifndef IMPULSE_GAIN_DEVICE_H
#define IMPULSE_GAIN_DEVICE_H

#include <opencv2/opencv.hpp>

#include <SingleInputDevice.hpp>

class ImpulseGainDevice : virtual public SingleInputDevice<cv::Point2f, cv::Point2f>
{
    private:

        // the impulse gain parameters
        float a, b, c;

        // the base class buffer
        CircularBuffer<cv::Point2f> *buffer;

        // the output value
        cv::Point2f new_value, output;


    public:

        // basic constructor
        ImpulseGainDevice(float a_, float b_, float c_) :
            SingleInputDevice<cv::Point2f, cv::Point2f>::SingleInputDevice(cv::Point2f(0.0, 0.0)), a(a_), b(b_), c(c_)
        {
            // get the buffer address
            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();
        }

        // device reset
        virtual void reset()
        {
            // set the output to zero
            output.x = 0.0;
            output.y = 0.0;

            // clear the entire input buffer
            buffer->clear();
        }

        // the main method
        virtual void run()
        {
            new_value = buffer->pop();

            // update the horizontal value
            if (c < new_value.x)
            {
                output.x = a*log(b*new_value.x + 1);

            }
            else if (-c > new_value.x)
            {
                output.x = -a*log(b*(-new_value.x) + 1);
            }
            else
            {
                output.x = 0.0;
            }

            // update the vertical value
            if (c < new_value.y)
            {
                output.y = a*log(b*new_value.y + 1);
            }
            else if (-c > new_value.y)
            {
                output.y = -a*log(b*(-new_value.y) + 1);
            }
            else
            {
                output.y = 0.0;
            }

            // send the output value to external devices
            DeviceOutput<cv::Point2f>::send(output);
        }
};

#endif
