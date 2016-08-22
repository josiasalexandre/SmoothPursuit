#ifndef SUBTRACT_VELOCITY_DEVICE_H
#define SUBTRACT_VELOCITY_DEVICE_H

#include <iostream>

#include <MultInputDevice.hpp>

template<typename T>
class SubtractVelocityDevice : virtual public MultInputDevice<T, T, 2> {

    public:

        // basic constructor
        SubtractVelocityDevice(T v_null) : MultInputDevice<T, T, 2>::MultInputDevice(v_null) {

        }

        // the main device method
        virtual void run() {

            // the first value
            T first = MultInputDevice<T, T, 2>::inputs[0].first.pop();

            // the second value
            T second = MultInputDevice<T, T, 2>::inputs[1].first.pop();

            // get the result
            T result = first - second;

            // send the result to external devices
            DeviceOutput<T>::send(result);

        };
        
        // implement the reset function
        virtual void reset() {
            
            // set the output to zero
            // clear the entire input buffer
            for (int i = 0; i < 2; i++) {

                // clear the entire buffer
                MultInputDevice<T, T, 2>::inputs[i].first.clear();

            }
            
            
        }

};

#endif
