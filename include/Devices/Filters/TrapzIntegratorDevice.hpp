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
            if (nullptr == buffer) {

                throw std::bad_alloc();

            }

        }

        // the main method
        virtual void run() {

            // get the new value
            new_value = buffer->pop();

            if (new_value != new_value || std::isinf(new_value.x) || std::isinf(new_value.y)) {
                std::cout << std::endl << "Trapz recebeu coisa errada: " << std::endl;
                std::cout << std::endl << "Old value: " << old_value;
                std::cout << std::endl << "New value: " << new_value;
                std::cout << std::endl  << "Output: " << output;
                assert(false);
            }

            // update the output value
            output = output + 0.001*0.5*(new_value + old_value);

            if (output != output || std::isinf(output.x) || std::isinf(output.y))
            {
                std::cout << std::endl << "Trapz produziu coisa errada: " << output << std::endl;
                std::cout << std::endl << "Old value: " << old_value;
                std::cout << std::endl << "New value: " << new_value;
                std::cout << std::endl  << "Output: " << output;

                assert(false);
            }

            // save the new value
            old_value = new_value;

            // send the output to external devices
            DeviceOutput<cv::Point2f>::send(output);

        }

};

#endif
