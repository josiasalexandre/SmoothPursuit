#ifndef FIR_DEVICE_H
#define FIR_DEVICE_H

#include <cmath>

#include <opencv2/opencv.hpp>

#include <SingleInputDevice.hpp>

enum FilterType {LOW_PASS, HIGH_PASS, BAND_PASS, BAND_STOP};
enum WindowType {RECTANGULAR_WINDOW, HAMMING_WINDOW, BARTLETT_WINDOW, HANNING_WINDOW, BLACKMAN_WINDOW};

template<unsigned int TAPS>
class FIRDevice : virtual public SingleInputDevice<cv::Point2f, cv::Point2f> {

    private:

        CircularBuffer<cv::Point2f> *buffer;

        // the circular base class buffer
        cv::Point2f special_buffer[TAPS];

        // the output
        cv::Point2f output;

        // coeffs
        double coeff[TAPS];

        // indexes
        int offset, buffer_index, coeff_index;

        // clear the special buffer
        void clear_special_buffer() {

            for (int i = 0; i < TAPS; i++) {

                // get the buffer
                special_buffer[i].x = 0.0;
                special_buffer[i].y = 0.0;

            }

        }

        // update the coeff by the WindowType
        void update_coeff(int halfLength, WindowType w_type) {

            int m = TAPS -1;
            double m_2 = 0.5*m;
            float normalization = 0.0;

            double val, tmp;

            switch (w_type) {
                case BARTLETT_WINDOW:
                    for (int n = 0; n <= halfLength; n++) {

                        val = 1.0 - 2.0 * (m_2 - n)/m;
                        coeff[n] *= val;
                        coeff[TAPS-n-1] *= val;

                    }
                    break;
                case HANNING_WINDOW:
                    for (int n = 0; n <= halfLength; n++) {

                        val = 0.5 - 0.5 * std::cos(2.0 * M_PI * n / (TAPS-1));
                        coeff[n] *= val;
                        coeff[TAPS-n-1] *= val;

                    }
                    break;
                case HAMMING_WINDOW:

                    for (int n = 0; n <= halfLength; n++) {

                        val = 0.54 - 0.46 * (std::cos(2.0 * M_PI * n / (TAPS -1)));
                        coeff[n] *= val;
                        coeff[TAPS-n-1] *= val;

                    }
                    break;
                case BLACKMAN_WINDOW:
                    for (int n = 0; n <= halfLength; n++) {

                        val = 0.42 - 0.5 * std::cos(2.0 * M_PI * n / (2*halfLength)) + 0.08 * std::cos(4.0 * M_PI * n / (2*halfLength));
                        coeff[n] *= val;
                        coeff[TAPS-n-1] *= val;

                    }
                    break;
                default:
                    break;
            }


            for (int i = 0; i < TAPS; i++) {
                normalization += coeff[i];
            }

            normalization = 1.0/normalization;

            for (int i = 0; i < TAPS; i++) {
                coeff[i] *= normalization;
            }

        }

    public:

        // the basic constructor for low pass and high pass fir filters
        FIRDevice(double sampFreq, double transFreq, FilterType f_type, WindowType w_type) :
            SingleInputDevice<cv::Point2f, cv::Point2f>::SingleInputDevice(cv::Point2f(0.0, 0.0)),
            offset(0), buffer(nullptr)
        {

            // Calculate the normalised transistion frequency. As transFreq should be
            // less than or equal to sampFreq / 2, ft should be less than 0.5
            double ft = transFreq / sampFreq;
            if (0.5 <= ft) {
                throw std::invalid_argument("The transition frequency must be lesser than the Nyquist frequency");
            }

            // get the middle
            double m_2 = 0.5*(TAPS - 1);

            // get the middle
            int halfLength = TAPS/2;

            double val;

            // verify the center TAP
            if (halfLength*2 != TAPS) {

                val = 2.0*ft;

                // If we want a high pass filter, subtract sinc function from a dirac pulse
                if (HIGH_PASS == f_type) {

                    val = 1.0 - val;

               }

                coeff[halfLength] = val;

            } else if (HIGH_PASS == f_type) {

                // problem!!!!
                throw std::invalid_argument("For high pass filter, coeff length must be odd");

            }

            if (HIGH_PASS == f_type) {

                ft = -ft;
            }

            // Calculate taps
            // Due to symmetry, only need to calculate half the coeff
            for (int n = 0; n < halfLength; n++) {

                // get the next value
                val = sin(2.0*M_PI*ft*(n-m_2))/(M_PI*(n-m_2));
                coeff[n] = val;
                coeff[TAPS-n-1] = val;

            }

            // update the coeff based on the WindowType
            update_coeff(halfLength, w_type);

            // clear the entire buffer
            clear_special_buffer();

            // get the buffer address
            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();
            if (nullptr == buffer) {

                throw std::bad_alloc();

            }

        }

        // the basic constructor for band pass and band stop fir filters
        FIRDevice(double sampFreq, double trans1Freq, double trans2Freq, FilterType f_type, WindowType w_type) :
            SingleInputDevice<cv::Point2f, cv::Point2f>::SingleInputDevice(cv::Point2f(0.0, 0.0)),
            offset(0), buffer(nullptr)
        {

            // Calculate the normalised transistion frequency. As transFreq should be
            // less than or equal to sampFreq / 2, ft should be less than 0.5
            double ft1 = trans1Freq / sampFreq;
            double ft2 = trans2Freq / sampFreq;
            if (0.5 <= ft1 || 0.5 <= ft2) {
                throw std::invalid_argument("The transition frequencies must be lesser than the Nyquist frequency");
            }

            // get the middle
            double m_2 = 0.5*(TAPS - 1);

            // get the middle
            int halfLength = TAPS/2;

            double val1, val2;

            // verify the center TAP
            if (halfLength*2 != TAPS) {

                double val = 2.0*(ft2 - ft1);

                // If we want a high pass filter, subtract sinc function from a dirac pulse
                if (BAND_STOP == f_type) {

                    val = 1.0 - val;

                }

                coeff[halfLength] = val;

            } else if (HIGH_PASS == f_type) {

                // problem!!!!
                throw std::invalid_argument("For band pass and band stop filters, coeff length must be odd");

            }

            // Swap transition points if Band Stop
            if (BAND_STOP == f_type) {
                double tmp = ft1;
                ft1 = ft2; ft2 = tmp;
            }

            // Calculate taps
            // Due to symmetry, only need to calculate half the coeff
            for (int n = 0; n < halfLength; n++) {

                val1 = sin(2.0*M_PI*ft1*(n-m_2))/(M_PI*(n-m_2));
                val2 = sin(2.0*M_PI*ft2*(n-m_2))/(M_PI*(n-m_2));

                coeff[n] = val2 - val1;
                coeff[TAPS-n-1] = val2 - val1;

            }

            // update the coeff based on the WindowType
            update_coeff(halfLength, w_type);

            // clear the entire buffer
            clear_special_buffer();

             // get the buffer
            buffer = SingleInputDevice<cv::Point2f, cv::Point2f>::get_buffer();
            if (nullptr == buffer) {

                throw std::bad_alloc();

            }


        }

        //
        virtual void run() {

            // save the new sample
            cv::Point2f poped = buffer->pop();
            if (poped.x != poped.x || std::isinf(poped.x)) {
                std::cout << std::endl << "FIR device recebeu coisa errada no x" << std::endl;
                std::cout << std::endl << poped << std::endl;
                assert(poped == poped);
            }
            if (poped.y != poped.y || std::isinf(poped.y)) {
                std::cout << std::endl << "FIR device recebeu coisa errada no y" << std::endl;
                std::cout << std::endl << poped << std::endl;
                assert(poped == poped);
            }

            special_buffer[offset] = poped;

            cv::Point2f tmp(0.0, 0.0);

            // the null value
            output.x = 0.0;
            output.y = 0.0;

            // get the actual index
            buffer_index = offset;

            // reset the coeff index
            coeff_index = 0;

            // first cycle
            while(0 <= buffer_index) {

                // convolution
                output += special_buffer[buffer_index--]*coeff[coeff_index++];

            }

            // get the last index
            buffer_index = TAPS - 1;

            // second cycle
            while(TAPS > coeff_index) {

                // convolution
                output += special_buffer[buffer_index--]*coeff[coeff_index++];

            }

            if (output != output || std::isinf(output.x) || std::isinf(output.y)) {

                std::cout << std::endl << "O problema está no fir device! Produziu: " << output << std::endl;

                assert(false);

            }

            // increments the offset and verify the overlapping case
            if (TAPS <= ++offset) {

                // reset the
                offset = 0;

            }



            // send the
            DeviceOutput<cv::Point2f>::send(output);

        }

};

#endif
