#ifndef TRAPEZOID_INTEGRATOR_DEVICE_H
#define TRAPEZOID_INTEGRATOR_DEVICE_H

#include <opencv2/opencv.hpp>

#include <SingleInputDevice.hpp>

class TrapzIntegratorDevice : virtual public SingleInputDevice<cv::Point2f, cv::Point2f> {

    private:

        // values
        cv::Point2f old_value, new_value, output;

        // base class buffer
        CircularBuffer<cv::Point2f> *buffer;

    public:

        // basic constructor
        TrapzIntegratorDevice(cv::Point2f v_null) :
            SingleInputDevice<cv::Point2f, cv::Point2f>::SingleInputDevice(v_null),
            old_value(0.0, 0.0), new_value(0.0, 0.0), output(0.0, 0.0), buffer(nullptr)
        {

            // get the buffer addres
            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();

        }

        // the main method
        virtual void run() {

            // get the new value
            new_value = buffer->pop();

            // update the output value
            output = output + 0.5*(new_value + old_value);

            // save the new value
            old_value = new_value;

            // send the output to external devices
            DeviceOutput<cv::Point2f>::send(output);

        }

};

#endif
