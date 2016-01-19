#ifndef MULT_INPUT_DEVICE_H
#define MULT_INPUT_DEVICE_H

#include <DeviceInput.hpp>
#include <DeviceOutput.hpp>

template<typename A, typename B, unsigned int N>
class MultInputDevice : virtual public DeviceInput<A>, virtual public DeviceOutput<B> {

    protected:

        // the device input buffer vector
        DeviceInputBuffer<A> inputs[N];

    public:

        // basic constructor
        MultInputDevice(A v_null) {

            for (int i = 0; i < N; i++) {

                // set the null value
                inputs[i].first.set_null_value(v_null);

                // reset the DeviceOutput pointers
                inputs[i].second = nullptr;

            }

        }

        // basic destructor
        virtual ~MultInputDevice() {

            for (int i = 0; i < N; i++) {

                if (nullptr != inputs[i].second) {

                    // build the DeviceInputBufferRef
                    DeviceInputBufferRef<A> buffer(&inputs[i].first, static_cast<DeviceInput<A> *>(this));

                    // remove the current device's input from the external device's output'
                    inputs[i].second->remove_output(buffer);

                    // set the external device pointer to nullptr, avoiding undesired deletion
                    inputs[i].second = nullptr;

                }

            }

        }

        // connect two devices
        virtual void connect(BaseDevice *dev) {

            if (nullptr != dev && this != dev) {

                // try to dowcast to DeviceOutput pointer
                DeviceOutput<A> *out = dynamic_cast<DeviceOutput<A> *>(dev);

                // try to dowcast to DeviceInput pointer
                DeviceInput<B> *in = dynamic_cast<DeviceInput<B> *>(dev);

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
        virtual void disconnect(BaseDevice *dev) {

            if (nullptr != dev && this != dev) {

                // try to disconnect the external device from this current input
                remove_signal_source(dev);

                // try to dowcast to DeviceInput pointer
                DeviceInput<B> *in = dynamic_cast<DeviceInput<B> *>(dev);

                if (nullptr != in) {

                    // try to disconnect this output device from the external device's input(s)
                    in->remove_signal_source(this);

                }

            }

        }

        // add a signal source to the current input
        virtual void add_signal_source(BaseDevice *dev) {

            if (nullptr != dev && this != dev) {

                // try to dowcast to DeviceOutput pointer
                DeviceOutput<A> *out = dynamic_cast<DeviceOutput<A> *>(dev);

                if (nullptr != out) {

                    // get the next valid input socket
                    for (int i = 0; i < N; i++) {

                        // found an empty input!
                        if (nullptr == inputs[i].second) {

                            // build the DeviceInputBufferRef
                            DeviceInputBufferRef<A> buffer(&inputs[i].first, static_cast<DeviceInput<A> *>(this));

                            // update the [i] external device pointer
                            inputs[i].second = out;

                            // updates the external device pointer
                            out->add_output(buffer);

                            // add just one time
                            return;

                        }

                    }

                    // warning /* TODO */


                }

            }

        }

        // add a signal source to the current input
        virtual void add_signal_source(BaseDevice *dev, unsigned int n) {

            if (nullptr != dev && this != dev && 0 <= n && N > n) {

                // try to dowcast to DeviceOutput pointer
                DeviceOutput<A> *out = dynamic_cast<DeviceOutput<A> *>(dev);

                if (nullptr != out) {

                    // build the DeviceInputBufferRef
                    DeviceInputBufferRef<A> buffer(&inputs[n].first, static_cast<DeviceInput<A> *>(this));

                    // update the input socket
                    if (nullptr != inputs[n].second) {

                        inputs[n].second->remove_output(buffer);

                        inputs[n].second = nullptr;

                    }

                    // update the [i] external device pointer
                    inputs[n].second = out;

                    // updates the external device pointer
                    out->add_output(buffer);

                }

            }

        }

        // remove a signal source from the current inputs
        virtual void remove_signal_source(BaseDevice *dev) {

            if (nullptr != dev && this != dev) {

                // try to dowcast to DeviceOutput pointer
                DeviceOutput<A> *out = dynamic_cast<DeviceOutput<A> *>(dev);

                if (nullptr != out) {

                    // try to find the output pointer
                    for (int i = N - 1; i >= 0; i--) {

                        if (out == inputs[i].second) {

                            // build the DeviceInputBufferRef
                            DeviceInputBufferRef<A> buffer(&inputs[i].first, static_cast<DeviceInput<A> *>(this));

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
        virtual void disconnect_signal_source(BaseDevice *dev) {

            if (nullptr != dev && this != dev) {

                // try to dowcast to DeviceOutput pointer
                DeviceOutput<A> *out = dynamic_cast<DeviceOutput<A> *>(dev);

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

        // get the desired buffer
        virtual CircularBuffer<A>* get_buffer(unsigned int n) {

            if (N > n && 0 <= n) {

                return &inputs[n].first;

            }

            return nullptr;

        }
};

#endif
