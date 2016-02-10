#ifndef FIRST_DIFFERENTIATOR_DEVICE_H
#define FIRST_DIFFERENTIATOR_DEVICE_H

#include <opencv2/opencv.hpp>

#include <SingleInputDevice.hpp>

class FirstDifferentiatorDevice : virtual public SingleInputDevice<cv::Point2f, cv::Point2f> {

    private:

        // the special buffer
        cv::Point2f new_value, old_value, output;

        // base class circular buffer pointer
        CircularBuffer<cv::Point2f> *buffer;


    public:

        // basic constructor
        FirstDifferentiatorDevice(cv::Point2f v_null) :
            SingleInputDevice<cv::Point2f, cv::Point2f>::SingleInputDevice(v_null),
            old_value(0.0, 0.0), buffer(nullptr)
        {

            // get the base class circular buffer
            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();
            if (nullptr == buffer) {

                throw std::bad_alloc();

            }

        }

        // device reset
        virtual void reset() {

            // set the output to zero
            output.x = 0.0;
            output.y = 0.0;

            // set the old value to zero
            old_value.x = 0.0;
            old_value.y = 0.0;

            // clear the entire input buffer
            buffer->clear();

        }
        // the main method
        virtual void run() {

            // get the new value
            new_value = buffer->pop();

            // computes the output
            output = new_value - old_value;

            // save the new value
            old_value = new_value;

            // send the value to external devices
            DeviceOutput<cv::Point2f>::send(output);


        }

};

#endif
