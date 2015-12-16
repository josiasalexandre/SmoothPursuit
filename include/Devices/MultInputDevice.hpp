#ifndef MULT_INPUT_DEVICE_H
#define MULT_INPUT_DEVICE_H

#include <BaseDevice.hpp>
#include <DeviceInput.hpp>
#include <DeviceOutput.hpp>
#include <boost/concept_check.hpp>

template<typename T, unsigned int N>
class MultInputDevice : virtual public DeviceInput<T>, virtual public DeviceOutput<T> {

    private:

        // the device input buffer vector
        DeviceInputBuffer<T> inputs[N];

    public:

        // basic constructor
        MultInputDevice() {

            for (int i = 0; i < N; i++) {

                // reset the DeviceOutput pointers
                inputs[i].second = nullptr;

            }

        }

        // basic destructor
        ~MultInputDevice() {

            for (int i = 0; i < N; i++) {

                if (nullptr != inputs[i].second) {

                    // build the DeviceInputBufferRef
                    DeviceInputBufferRef<T> buffer(&inputs[i].first, static_cast<DeviceInput<T> *>(this));

                    // remove the current device's input from the external device's output'
                    inputs[i].second->remove_output(buffer);

                    // set the external device pointer to nullptr, avoiding undesired deletion
                    inputs[i].second = nullptr;

                }

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

                    // get the next valid input socket
                    for (int i = 0; i < N; i++) {

                        // found an empty input!
                        if (nullptr == inputs[i].second) {

                            // build the DeviceInputBufferRef
                            DeviceInputBufferRef<T> buffer(&inputs[i].first, static_cast<DeviceInput<T> *>(this));

                            // update the [i] external device pointer
                            inputs[i].second = out;

                            // updates the external device pointer
                            out->add_output(buffer);

                            // add just one time
                            return;

                        }

                    }

                }

            }

        }

        // remove a signal source from the current inputs
        virtual void remove_signal_source(BaseDevice<T> *dev) {

            if (nullptr != dev) {

                // try to dowcast to DeviceOutput pointer
                DeviceOutput<T> *out = dynamic_cast<DeviceOutput<T> *>(dev);

                if (nullptr != out) {

                    // try to find the output pointer
                    for (int i = N - 1; i >= 0; i--) {

                        if (out == inputs[i].second) {

                            // build the DeviceInputBufferRef
                            DeviceInputBufferRef<T> buffer(&inputs[i].first, static_cast<DeviceInput<T> *>(this));

                            // remove the current device's input from the external device's output'
                            inputs[i].second->remove_output(buffer);

                            // set the external device pointer to nullptr, avoiding undesired deletion
                            inputs[i].second = nullptr;

                        }

                    }

                }

            }

        }

        // disconnect the a signal source from the current inputs
        virtual void disconnect_signal_source(BaseDevice<T> *dev) {

            if (nullptr != dev) {

                // try to dowcast to DeviceOutput pointer
                DeviceOutput<T> *out = dynamic_cast<DeviceOutput<T> *>(dev);

                if (nullptr != out) {

                    // try to find the output pointer
                    for (int i = N - 1; i >= 0; i--) {

                        if (out == inputs[i].second) {

                            // set the external device pointer to nullptr, avoiding undesired deletion
                            inputs[i].second = nullptr;

                        }

                    }

                }

            }

        }

};

#endif
