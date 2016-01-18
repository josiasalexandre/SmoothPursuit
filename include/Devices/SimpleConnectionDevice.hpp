#ifndef SINGLE_CONNECTION_DEVICE_H
#define SINGLE_CONNECTION_DEVICE_H

#include <vector>

#include <SingleInputDevice.hpp>

template<typename T>
class SimpleConnectionDevice : virtual public SingleInputDevice<std::vector<T>, T> {

    private:

        // base class buffer
        CircularBuffer<T> *buffer;

    public:

        // basic constructor
        SimpleConnectionDevice(T v_null):
            SingleInputDevice<T, T>::SingleInputDevice(v_null), buffer(nullptr)
        {

            // get the buffer address
            buffer = SingleInputDevice<T, T>::get_buffer();

        }

        // the run method
        virtual void run() {

            // get the first element
            std::vector<T> first = buffer->pop();

            int first_size = first.size();

            for (int i = 0; i < first_size; i++) {

                // send the output to the external devices
                DeviceOutput<T>::send(first[i]);

            }

        }

};

#endif
