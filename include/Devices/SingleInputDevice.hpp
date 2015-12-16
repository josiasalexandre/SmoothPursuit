#ifndef SINGLE_INPUT_DEVICE_H
#define SINGLE_INPUT_DEVICE_H

#include <BaseDevice.hpp>
#include <DeviceInput.hpp>
#include <DeviceOutput.hpp>

template<typename T>
class SingleInputDevice : virtual public DeviceInput<T>, virtual public DeviceOutput<T> {

    private:

        // the device input buffer
        DeviceInputBuffer<T> input;

    public:

        // basic constructor
        SingleInputDevice() {

            // set the DeviceOutput pointer to null
            input.second = nullptr;

        }

        // basic destructor
        ~SingleInputDevice() {

            if (nullptr != input.second) {

                // build the DeviceInputBufferRef
                DeviceInputBufferRef<T> buffer(&input.first, static_cast<DeviceInput<T> *>(this));

                // remove the current device's input from the external device's output'
                input.second->remove_output(buffer);

                // set the external device pointer to nullptr, avoiding undesired deletion
                input.second = nullptr;

            }

        }

        // connect two devices
        virtual void connect(BaseDevice<T> *dev) {

            if (nullptr != dev) {

                // try to dowcast to DeviceOutput pointer
                DeviceOutput<T> *out = dynamic_cast<DeviceOutput<T> *>(dev);

                // try to dowcast to DeviceInput pointer
                DeviceInput<T> *in = dynamic_cast<DeviceInput<T> *>(dev);

                if (nullptr != in) {

                    // connect the current device's output to the external device's input
                    in->add_signal_source(this);

                } else if (nullptr != out) {

                    // connec the external device's output to this current device's input
                    // reverse case
                    add_signal_source(dev);

                }

            }

        }

        // disconnect two devices
        virtual void disconnect(BaseDevice<T> *dev) {

            // try to disconnect the external device from this current input
            remove_signal_source(dev);

            // try to dowcast to DeviceInput pointer
            DeviceInput<T> *in = dynamic_cast<DeviceInput<T> *>(dev);

            if (nullptr != in) {

                // try to disconnect this output device from the external device's input(s)
                in->remove_signal_source(this);

            }

        }

        // the main device method
        virtual void run() {}

        // add a signal source to the current input
        virtual void add_signal_source(BaseDevice<T> *dev) {

            if (nullptr != dev) {

                // try to dowcast to DeviceOutput pointer
                DeviceOutput<T> *out = dynamic_cast<DeviceOutput<T> *>(dev);

                if (nullptr != out) {

                    // build the DeviceInputBufferRef
                    DeviceInputBufferRef<T> buffer(&input.first, static_cast<DeviceInput<T> *>(this));

                    if (nullptr != input.second) {

                        input.second->remove_output(buffer);

                    }

                    // update the current external device pointer
                    input.second = out;

                    // updates the external device pointer
                    out->add_output(buffer);

                }

            }

        }

        // remove a signal source from the current inputs
        virtual void remove_signal_source(BaseDevice<T> *dev) {

            if (nullptr != dev) {

                // try to dowcast to DeviceOutput pointer
                DeviceOutput<T> *out = dynamic_cast<DeviceOutput<T> *>(dev);

                if (out == input.second) {

                    // build the DeviceInputBufferRef
                    DeviceInputBufferRef<T> buffer(&input.first, this);

                    // remove the current device's input from the external device's output'
                    input.second->remove_output(buffer);

                    // set the external device pointer to nullptr, avoiding undesired deletion
                    input.second = nullptr;

                }

            }

        }

        // disconnect the a signal source from the current inputs
        virtual void disconnect_signal_source(BaseDevice<T> *dev) {

            if (nullptr != dev) {

                // try to dowcast to DeviceOutput pointer
                DeviceOutput<T> *out = dynamic_cast<DeviceOutput<T> *>(dev);

                if (out == input.second) {

                    // set the external device pointer to nullptr, avoiding undesired deletion
                    input.second = nullptr;

                }

            }

        }

};

#endif
