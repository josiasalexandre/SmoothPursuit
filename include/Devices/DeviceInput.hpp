#ifndef GENERAL_DEVICE_INPUT_INTERFACE_H
#define GENERAL_DEVICE_INPUT_INTERFACE_H

#include <vector>
#include <utility>

#include <CircularBuffer.hpp>
#include <BaseDevice.hpp>

template<typename T>
class DeviceInput : virtual public BaseDevice<T> {

    public:

        // add a signal source to the current input
        virtual void add_signal_source(BaseDevice<T> *) =0;

        // remove a signal source from the current inputs
        virtual void remove_signal_source(BaseDevice<T> *) =0;

        // disconnect the a signal source from the current inputs
        virtual void disconnect_signal_source(BaseDevice<T> *) =0;
};

template<typename T>
using DeviceInputBufferRef = std::pair<CircularBuffer<T> *, DeviceInput<T> *>;

#endif
