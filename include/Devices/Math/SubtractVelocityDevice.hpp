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

            if (first == first && second == second && result != result) {
                std::cout << std::endl << "O problema está aqui" << std::endl;
                std::cout << std::endl << "first: " << first << std::endl;
                std::cout << std::endl << "second: " << second << std::endl;
                std::cout << std::endl << "result: " << result << std::endl;
                assert(result == result);   
            }

            // send the result to external devices
            DeviceOutput<T>::send(result);

//             std::cout << std::endl;
//             std::cout << std::endl << "Valor first: " << first << std::endl;
//             std::cout << std::endl << "Valor second: " << second << std::endl;
//             std::cout << std::endl << "result: " << result << std::endl;
//             if (second.x > 0 || second.y > 0) {
//             }

        };

};

#endif
