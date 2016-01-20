#ifndef ADD_SIGNALS_DEVICE_H
#define ADD_SIGNALS_DEVICE_H

#include <iostream>

#include <MultInputDevice.hpp>

template<typename T, unsigned int N>
class AddSignalsDevice : virtual public MultInputDevice<T, T, N> {

    private:

    public:

        AddSignalsDevice(T v_null) : MultInputDevice<T, T, N>::MultInputDevice(v_null) {

            if (1 > N) {

                throw std::invalid_argument("usage: AddSignalsDevice<T, N>(T v_null) -> N must be greater than zero");

            }

        }

        // the main device method
        virtual void run() {

            // get the null value
            T tmp = MultInputDevice<T, T, N>::inputs[0].first.get_null_value();

            // sum all the values
            for (int i = 0; i < N; i++) {

                if (!MultInputDevice<T, T, N>::inputs[i].first.empty()) {

                    // computes the new value
                    tmp = tmp + MultInputDevice<T, T, N>::inputs[i].first.pop();

                }

            }

            // send the value to all external devices
            if (0 < DeviceOutput<T>::output.size()) {

                // lock the output pointers
                DeviceOutput<T>::output_mutex.lock();

                int out_size = DeviceOutput<T>::output.size();

                for (int i = 0; i < out_size; i++) {

                    // send the value
                    DeviceOutput<T>::output[i].first->push(tmp);

                }

                // unlock the output pointers
                DeviceOutput<T>::output_mutex.unlock();


            }

        }

};

#endif
