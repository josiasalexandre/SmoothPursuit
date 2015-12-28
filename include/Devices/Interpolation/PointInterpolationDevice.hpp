#ifndef SIGNAL_INTERPOLATION_H
#define SIGNAL_INTERPOLATION_H

#include <vector>
#include <boost/concept_check.hpp>

#include <opencv2/opencv.hpp>

#include <SingleInputDevice.hpp>

enum Interpolation {LinearInterpolation, QuadraticInterpolation, SplineInterpolation};

class PointInterpolationDevice : virtual public SingleInputDevice<cv::Point2f, std::vector<cv::Point2f>> {

    private:

        // interpolation type
        Interpolation interpolation;

        // the interpolated signal
        std::vector<cv::Point2f> interpolated;

        // interpolation buffer size
        unsigned int interpolation_buffer_size;

        // sampling rate
        float sampling;

        // the frame rate
        float fps;

        // the period
        float period;

        // the old and the current point, the iterpolation consider this points
        cv::Point2f old_point, current_point, interp_point;

        CircularBuffer<cv::Point2f> *buffer;

        // helpers
        float a1, a2, b1, b2, step;

        // private methods
        void linear_interpolation() {

            // x interpolation
            a1 = (current_point.x - old_point.x)*fps;

            b1 = old_point.x;

            // y interpolation
            a2 = (current_point.y - old_point.y)*fps;

            b2 = old_point.y;

            interpolated.push_back(old_point);

            step = 0.0 + sampling;

            while(step < period) {

                interp_point.x = a1*step + b1;
                interp_point.y = a2*step + b2;

                interpolated.push_back(interp_point);

                // next step
                step += sampling;

            }

        }

        // quadratic interpolation
        void quad_interpolation() {

            /* TODO */

        }

        // spline interpolation
        void spline_iterpolation() {

            /* TODO */

        }

    public:

        // basic constructor
        PointInterpolationDevice() : interpolation(LinearInterpolation), interpolated(0), sampling(0.001), fps(30.0), period(1.0/30.0), buffer(nullptr), interpolation_buffer_size(2) {

            CircularBuffer<cv::Point2f> *buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();

            cv::Point2f zero(0.0, 0.0);

            buffer->set_null_value(zero);

        }

        // basic constructor
        PointInterpolationDevice(Interpolation interp) : interpolation(interp), interpolated(0), sampling(0.001), fps(30), period(1.0/30.0), buffer(nullptr){

            CircularBuffer<cv::Point2f> *buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();

            cv::Point2f zero(0.0, 0.0);

            buffer->set_null_value(zero);

            if (LinearInterpolation == interpolation) {

                interpolation_buffer_size = 2;

            } else if (QuadraticInterpolation == interpolation) {

                interpolation_buffer_size = 3;

            } else if (SplineInterpolation == interpolation) {

                interpolation_buffer_size = 4;

            }

        }

        // basic constructor
        PointInterpolationDevice(Interpolation interp, float sampling_freq, float frame_rate) :
                                                                                                interpolation(interp),
                                                                                                interpolated(0),
                                                                                                sampling(sampling_freq),
                                                                                                fps(frame_rate),
                                                                                                period(1.0/frame_rate),
                                                                                                buffer(nullptr)
        {

            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();

            cv::Point2f zero(0.0, 0.0);

            buffer->set_null_value(zero);

            if (LinearInterpolation == interpolation) {

                interpolation_buffer_size = 2;

            } else if (QuadraticInterpolation == interpolation) {

                interpolation_buffer_size = 3;

            } else if (SplineInterpolation == interpolation) {

                interpolation_buffer_size = 4;

            }

        }

        // now with the null interp_point set
        PointInterpolationDevice(cv::Point2f v_null) :
                                                        SingleInputDevice< cv::Point2f, cv::Point2f >::SingleInputDevice(v_null),
                                                        interpolation(LinearInterpolation),
                                                        interpolated(0),
                                                        sampling(1000),
                                                        fps(30),
                                                        period(1.0/30.0),
                                                        buffer(nullptr),
                                                        interpolation_buffer_size(2)
        {

            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();

        }

        // now with the null interp_point set
        PointInterpolationDevice(Interpolation interp, cv::Point2f v_null) :
                                                                                SingleInputDevice< cv::Point2f, cv::Point2f >::SingleInputDevice(v_null),
                                                                                interpolation(interp),
                                                                                interpolated(0),
                                                                                sampling(1000),
                                                                                fps(30),
                                                                                period(1.0/30.0),
                                                                                buffer(nullptr)
        {

            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();

            if (LinearInterpolation == interpolation) {

                interpolation_buffer_size = 2;

            } else if (QuadraticInterpolation == interpolation) {

                interpolation_buffer_size = 3;

            } else if (SplineInterpolation == interpolation) {

                interpolation_buffer_size = 4;

            }

        }

        // basic destructor
        virtual ~PointInterpolationDevice() {}

        // update the sampling frequency
        void set_sampling_frequency(float sampling_freq) {

            sampling = sampling_freq;

        }

        // get the sampling frequency
        float get_sampling_frequency() {

            return sampling;

        }

        // update the frame rate
        void set_fps(float frame_rate) {

            fps = frame_rate;

            period = 1.0/frame_rate;

        }

        virtual void run() {

            // get the images from the the input buffer
            current_point = buffer->pop();

            if (LinearInterpolation == interpolation) {

                linear_interpolation();
            }

            DeviceOutput<std::vector<cv::Point2f>>::send(interpolated);

            // clear the interpolated vector
            interpolated.clear();

            old_point = current_point;

        }

};

#endif
