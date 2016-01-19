#ifndef LOW_PASS_EXPONENTIAL_DEVICE_H
#define LOW_PASS_EXPONENTIAL_DEVICE_H

#include <opencv2/opencv.hpp>

#include <SingleInputDevice.hpp>

// simpler IIR filter implementation -> the exponencial filter
class LowPassExponentialDevice : virtual public SingleInputDevice<cv::Point2f, cv::Point2f> {

    private:

        // the time constant
        float alpha, alpha_1;

        // the sampling rate
        float T;

        // the tau param
        float tau;

        // the last value and the filtered one
        cv::Point2f old_sample, filtered;

        // a pointer to the base class buffer =( | * need to avoid protected? |
        CircularBuffer<cv::Point2f> *buffer;

    public:

        // basic constructor
        LowPassExponentialDevice() :
            SingleInputDevice<cv::Point2f, cv::Point2f>::SingleInputDevice(cv::Point2f(0.0, 0.0)),
            T(0.001), tau(0.015), old_sample(0.0, 0.0), buffer(nullptr)
        {


            //
            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();
            if (nullptr == buffer) {

                throw std::bad_alloc();

            }

            alpha =  std::exp(-T/tau);
            alpha_1 = 1-alpha;

        }

        // basic constructor
        LowPassExponentialDevice(float sampling_rate, float time) :
            SingleInputDevice<cv::Point2f, cv::Point2f>::SingleInputDevice(cv::Point2f(0.0, 0.0)),
            T(sampling_rate), tau(time), old_sample(0.0, 0.0), buffer(nullptr)
        {

            // get the buffer
            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();
            if (nullptr == buffer) {

                throw std::bad_alloc();

            }

            alpha =  std::exp(-T/tau);
            alpha_1 = 1-alpha;

        }

        // basic constructor
        LowPassExponentialDevice(cv::Point2f v_null) :
            SingleInputDevice< cv::Point2f, cv::Point2f>::SingleInputDevice(v_null),
            T(0.001),
            tau(0.015),
            old_sample(0.0, 0.0),
            buffer(nullptr)
        {

            alpha =  std::exp(-T/tau);
            alpha_1 = 1-alpha;

            // get the buffer
            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();
            if (nullptr == buffer) {

                throw std::bad_alloc();

            }

        }

        // basic constructor
        LowPassExponentialDevice(cv::Point2f v_null, float sampling_rate, float time) :
            SingleInputDevice< cv::Point2f, cv::Point2f>::SingleInputDevice(v_null),
            T(sampling_rate),
            tau(time),
            old_sample(0.0, 0.0),
            buffer(nullptr)
        {

            alpha =  std::exp(-T/tau);
            alpha_1 = 1-alpha;

            // get the buffer
            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();
            if (nullptr == buffer) {

                throw std::bad_alloc();

            }

        }

        // @override the run method
        virtual void run() {

            // get the value
            filtered = buffer->pop();

            // filtering
            filtered = alpha*old_sample + alpha_1*filtered;

            // save the actual filtered value
            old_sample = filtered;

            // send to all external connected devices
            DeviceOutput<cv::Point2f>::send(filtered);

        }

        // update the tau
        void update_tau(float time) {

            tau = time;

            alpha = std::exp(-T/tau);

            alpha_1 = 1 - alpha;

        }

        // update the sampling rate
        void update_sampling_rate(float sampling_rate) {

            T = sampling_rate;

            alpha = std::exp(-T/tau);

            alpha_1 = 1 - alpha;

        }

};

#endif
