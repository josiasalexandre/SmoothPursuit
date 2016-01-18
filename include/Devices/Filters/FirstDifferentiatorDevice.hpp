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


        // the main method
        virtual void run() {

            // get the new value
            new_value = buffer->pop();

            output = new_value - old_value;

            old_value = new_value;

            // send the value to external devices
            DeviceOutput<cv::Point2f>::send(output);


        }

};

#endif
