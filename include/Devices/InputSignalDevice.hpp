#ifndef INPUT_SIGNAL_DEVICE_H
#define INPUT_SIGNAL_DEVICE_H

#include "DeviceOutput.hpp"

template<typename T>
class InputSignalDevice : virtual public DeviceOutput<T> {

    public:

        // connect two devices
        virtual void connect(BaseDevice *dev) {

            if (nullptr != dev && this != dev) {

                // try to dowcast to DeviceInput pointer
                DeviceInput<T> *in = dynamic_cast<DeviceInput<T> *>(dev);

                if (nullptr != in) {

                    // connect the current device's output to the external device's input
                    in->add_signal_source(this);

                }

            }

        }

        // disconnect two devices
        virtual void disconnect(BaseDevice *dev) {

            if (nullptr != dev && this != dev) {

                // try to dowcast to DeviceInput pointer
                DeviceInput<T> *in = dynamic_cast<DeviceInput<T> *>(dev);

                if (nullptr != in) {

                    // try to disconnect this output device from the external device's input(s)
                    in->remove_signal_source(this);

                }
            }

        }

};

#endif
