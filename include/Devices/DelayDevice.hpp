#ifndef DELAY_DEVICE_H
#define DELAY_DEVICE_H

#include <opencv2/opencv.hpp>

#include <SingleInputDevice.hpp>

class DelayDevice : virtual public SingleInputDevice<cv::Point2f, cv::Point2f> {

    private:

        // the time delay
        float time;

        // the output value
        cv::Point2f output;

        // the base class buffer
        CircularBuffer<cv::Point2f> *buffer;

    public:

        // basic constructor
        DelayDevice(float t) :
            SingleInputDevice<cv::Point2f, cv::Point2f>::SingleInputDevice(cv::Point2f(0.0, 0.0)),
            buffer(nullptr), output(0.0), time(t)
        {

            // get the buffer address
            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();
            if (buffer == nullptr) {

                throw std::bad_alloc();

            }

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
            output.x = 0.0;
            output.y = 0.0;

            if (1.0 >= time) {

                // get the new value
                output = buffer->pop();

            } else {

                time -= 1.0;

            }

            // send the output to external devices
            DeviceOutput<cv::Point2f>::send(output);
//             std::cout << std::endl << "Output: " << output << std::endl;

        }

};

#endif
