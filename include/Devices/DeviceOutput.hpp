#ifndef GENERAL_DEVICE_OUTPUT_INTERFACE_H
#define GENERAL_DEVICE_OUTPUT_INTERFACE_H

#include <vector>
#include <mutex>

#include <BaseDevice.hpp>
#include <DeviceInput.hpp>

template<typename T>
class DeviceOutput : virtual public BaseDevice {

    private:

        // the external devices inputs are the current device outputs
        std::vector<DeviceInputBufferRef<T>> output;

        // the output mutex
        std::mutex output_mutex;

    public:

        // basic constructor
        DeviceOutput() : output(0) {}

        // basic destructor
        ~DeviceOutput() {

            int out_size = output.size();

            // lock the output vector
            output_mutex.lock();

            for (int i = 0; i < out_size; i++) {

                // set the CircularBuffer to null
                output[i].first = nullptr;

                // disconnects the current device's output from the external input
                output[i].second->disconnect_signal_source(this);

                // set the DeviceInput pointer to null
                output[i].second = nullptr;

            }

            // unlock the output vector
            output_mutex.unlock();

        }

        // add an external device's input as an valid output
        virtual void add_output(DeviceInputBufferRef<T> dev) {

            if (nullptr != dev.first && nullptr != dev.second) {

                // save the external device's input an pointer
                output.push_back(dev);

            }

        }

        // remove an output buffer from the current
        virtual void remove_output(DeviceInputBufferRef<T> dev) {

            if (nullptr != dev.first && nullptr != dev.second) {

                int out_size = output.size();

                // lock the output vector
                output_mutex.lock();

                for (int i = 0; i < out_size; i++) {

                    if (dev == output[i]) {

                        // set the pointers to null
                        output[i].first = nullptr;
                        output[i].second = nullptr;

                        // erase the element
                        output.erase(output.begin() + i);

                        // remove just the first one
                        return;

                    }

                }

                // unlock the output vector
                output_mutex.unlock();

            }

        }

        // remove_output() method overloaded
        virtual void remove_output(DeviceInput<T> *dev) {

            if (nullptr != dev) {

                int out_size = output.size();

                // lock the output vector
                output_mutex.lock();

                for (int i = 0; i < out_size; i++) {

                    if (dev == output[i].second) {

                        // set the pointers to null
                        output[i].first = nullptr;
                        output[i].second = nullptr;

                        // erase the element
                        output.erase(output.begin() + i);

                    }

                }

                // unlock the output vector
                output_mutex.unlock();

            }

        }

};

template<typename T>
using DeviceInputBuffer = std::pair<CircularBuffer<T>, DeviceOutput<T> *>;

#endif
