#ifndef LOW_PASS_FILTER_DEVICE_H
#define LOW_PASS_FILTER_DEVICE_H

#include <opencv2/opencv.hpp>

#include <SingleInputDevice.hpp>

#define TAPS 100

// FIR implementation - 100 taps from octave fir1(100. 0.05)
// considering the sampling rate at 0.001 and cut freq at 0.033333 s

class LowPassFilterDevice : virtual public SingleInputDevice<cv::Point2f, cv::Point2f> {

    private:

        // fir filter coefficients
        float coeff[TAPS];

        cv::Point2f special_buffer[TAPS];

        unsigned int offset, buffer_index, coeff_index;

    public:


        // basic constructor
        LowPassFilterDevice () : coeff(
                {
                    7.9426e-05, 6.9757e-05, 6.0743e-05, 5.1941e-05, 4.2996e-05, 3.3744e-05, 2.4308e-05, 1.5200e-05, 7.4246e-06, 2.5687e-06,
                    2.8838e-06, 1.1349e-05, 3.1713e-05, 6.8511e-05, 1.2705e-04, 2.1336e-04, 3.3414e-04, 4.9660e-04, 7.0837e-04, 9.7729e-04,
                    1.3112e-03, 1.7178e-03, 2.2042e-03, 2.7770e-03, 3.4417e-03, 4.2027e-03, 5.0627e-03, 6.0232e-03, 7.0834e-03, 8.2407e-03,
                    9.4904e-03, 1.0826e-02, 1.2237e-02, 1.3714e-02, 1.5242e-02, 1.6808e-02, 1.8393e-02, 1.9980e-02, 2.1550e-02, 2.3082e-02,
                    2.4558e-02, 2.5955e-02, 2.7255e-02, 2.8439e-02, 2.9490e-02, 3.0391e-02, 3.1130e-02, 3.1694e-02, 3.2075e-02, 3.2267e-02,
                    3.2267e-02, 3.2075e-02, 3.1694e-02, 3.1130e-02, 3.0391e-02, 2.9490e-02, 2.8439e-02, 2.7255e-02, 2.5955e-02, 2.4558e-02,
                    2.3082e-02, 2.1550e-02, 1.9980e-02, 1.8393e-02, 1.6808e-02, 1.5242e-02, 1.3714e-02, 1.2237e-02, 1.0826e-02, 9.4904e-03,
                    8.2407e-03, 7.0834e-03, 6.0232e-03, 5.0627e-03, 4.2027e-03, 3.4417e-03, 2.7770e-03, 2.2042e-03, 1.7178e-03, 1.3112e-03,
                    9.7729e-04, 7.0837e-04, 4.9660e-04, 3.3414e-04, 2.1336e-04, 1.2705e-04, 6.8511e-05, 3.1713e-05, 1.1349e-05, 2.8838e-06,
                    2.5687e-06, 7.4246e-06, 1.5200e-05, 2.4308e-05, 3.3744e-05, 4.2996e-05, 5.1941e-05, 6.0743e-05, 6.9757e-05, 7.9426e-05
                }
            ) , offset(0)
        {
            cv::Point2f zero(0.0, 0.0);
            for (int i = 0; i < TAPS; i++) { special_buffer[i] = zero; }
        }

        virtual void run() {}

        // filtering the input
        virtual cv::Point2f filter(cv::Point2f input)
        {
            // save the new sample
            special_buffer[offset] = input;

            cv::Pint2f output(0.0, 0.0);

            buffer_index = offset;
            coeff_index = 0;

            while(0 <= buffer_index)
            {
                output += special_buffer[buffer_index--]*coeff[coeff_index++];
            }

            buffer_index = TAPS - 1;

            while(TAPS > coeff_index)
            {
                output += special_buffer[buffer_index--]*coeff[coeff_index++];
            }

            // increments the offset and verify the overlapping case
            if (TAPS <= ++offset)
            {
                // reset the
                offset = 0;
            }
            return output;
        }

};


#endif
