#ifndef VELOCITY_INTERPOLATION_DEVICE_H
#define VELOCITY_INTERPOLATION_DEVICE_H

#include <opencv2/opencv.hpp>

#include <SingleInputDevice.hpp>

enum InterpolationType {LINEAR_INTERPOLATION, QUADRATIC_INTERPOLATION};

class VelocityInterpolationDevice : virtual public SingleInputDevice<cv::Point2f, std::vector<cv::Point2f>> {

    private:

        // the base class circular buffer
        CircularBuffer<cv::Point2f> *buffer;

        // the current and old value
        cv::Point2f current, old;

        // the interpolated signal
        std::vector<cv::Point2f> interpolated;

        // the interpolation flag
        InterpolationType type;

        // the sampling frequency and the fps
        float fs, fps;

        // the linear interpolation method
        void linear_interpolation() {

            unsigned int interp = (fs/fps) - 1;
            float a1, a2;
            cv::Point2f new_point;

            // x interpolation
            // save the first value
            interpolated.push_back(old);

            // get the inclination delta y / delta x
            a1 = (current.x - old.x);
            a2 = (current.y - old.y);

            std::cout << std::endl << "Interpolation -> a1: " << a1 << ", a2: " << a2 << std::endl;
            std::cout << std::endl << old << std::endl;
            std::cout << std::endl << current << std::endl;

            for (int i = 1; i <= interp; i++) {

                new_point.x = a1*(i*0.001) + old.x;
                new_point.y = a2*(i*0.001) + old.y;

                interpolated.push_back(new_point);

                std::cout << std::endl << "i: " << i << " -> " << new_point << std::endl;

            }

        }

    public:

        // basic constructor
        VelocityInterpolationDevice(InterpolationType interp_type, float fs_rate = 1000, float fr = 25) :
            SingleInputDevice<cv::Point2f, std::vector<cv::Point2f>>::SingleInputDevice(cv::Point2f(0.0, 0.0)),
            current(0.0, 0.0),
            old(0.0, 0.0),
            buffer(nullptr),
            type(interp_type), interpolated(0),
            fs(fs_rate),
            fps(fr)
        {

            // get the input buffer pointer
            buffer = SingleInputDevice<cv::Point2f, std::vector<cv::Point2f>>::get_buffer();

        }

        // the main method
        virtual void run() {

            // get the new value
            current = buffer->pop();

            //
            switch(type) {

                case LINEAR_INTERPOLATION:
                    linear_interpolation();
                    break;
                case QUADRATIC_INTERPOLATION:
                    break;
                default:
                    break;

            }

            // send the interpolated signal
            DeviceOutput<std::vector<cv::Point2f>>::send(interpolated);

            // update de old value
            old = current;

        }

};

#endif
