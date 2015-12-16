#ifndef GENERAL_BASE_DEVICE_H
#define GENERAL_BASE_DEVICE_H

#include <vector>

#include <CircularBuffer.hpp>

template<typename T>
class BaseDevice {

    public:

        // basic constructor
        BaseDevice() {}

        // connect two devices
        virtual void connect(BaseDevice *) =0;

        // disconnect two devices
        virtual void disconnect(BaseDevice *) = 0;

        // the main device method
        virtual void run() = 0;

};

#endif
