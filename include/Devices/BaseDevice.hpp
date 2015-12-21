#ifndef GENERAL_BASE_DEVICE_H
#define GENERAL_BASE_DEVICE_H

enum DeviceStatus { Device_ON, Device_OFF};

class BaseDevice {

    protected:

        DeviceStatus status;

    public:

        // basic constructor
        BaseDevice() : status(Device_OFF) {}

        virtual ~BaseDevice() {}

        // connect two devices
        virtual void connect(BaseDevice *) = 0;

        // disconnect two devices
        virtual void disconnect(BaseDevice *) = 0;

        // update the device status
        virtual void update_status(DeviceStatus ds) {

            status = ds;

        }

        // get the device status
        virtual DeviceStatus get_device_status() {

            return status;

        }

        // the main device method
        virtual void run() = 0;

};

#endif
