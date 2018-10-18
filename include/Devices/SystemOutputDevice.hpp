#ifndef SYSTEM_OUTPUT_DEVICE_H
#define SYSTEM_OUTPUT_DEVICE_H

#include <vector>
#include <stdexcept>

#include <DeviceInput.hpp>
#include <DeviceOutput.hpp>

template<typename T>
class SystemOutputDevice : virtual public DeviceInput<T> {

    private:

        // the device input buffer
        DeviceInputBuffer<T> input;

        // the output vector address
        std::vector<T> *output;

    public:

        // basic constructor
        SystemOutputDevice (std::vector<T> *out) : output(out)
        {
            if (nullptr == out) { throw std::invalid_argument("The output vector must be valid"); }
        }

        // basic destructor
        virtual ~SystemOutputDevice()
        {
            if (nullptr != input.second)
            {
                // build the DeviceInputBufferRef
                DeviceInputBufferRef<T> buffer(&input.first, static_cast<DeviceInput<T> *>(this));

                // remove the current device's input from the external device's output'
                input.second->remove_output(buffer);

                // set the external device pointer to nullptr, avoiding undesired deletion
                input.second = nullptr;
            }
        }

        // connect two devices
        virtual void connect(BaseDevice *dev)
        {
            if (nullptr != dev && this != dev)
            {
                // try to dowcast to DeviceOutput pointer
                DeviceOutput<T> *out = dynamic_cast<DeviceOutput<T> *>(dev);

                if (nullptr != out)
                {
                    // connect the external device's output to this current device's input
                    add_signal_source(dev);
                }
            }
        }

        // disconnect two devices
        virtual void disconnect(BaseDevice *dev)
        {
            if (nullptr != dev && this != dev)
            {
                // try to disconnect the external device from this current input
                remove_signal_source(dev);
            }
        }

        // device reset
        virtual void reset()
        {
            // set the output to zero
            output->clear();

            // clear the entire input buffer
            input.first.clear();
        }

        // the main device method
        virtual void run()
        {
            // get the first element
            T out = input.first.pop();

            // send to the output vector
            output->push_back(out);
        }

        // add a signal source to the current input
        virtual void add_signal_source(BaseDevice *dev)
        {
            if (nullptr != dev && this != dev)
            {
                // try to dowcast to DeviceOutput pointer
                DeviceOutput<T> *out = dynamic_cast<DeviceOutput<T> *>(dev);

                if (nullptr != out)
                {
                    // build the DeviceInputBufferRef
                    DeviceInputBufferRef<T> buffer(&input.first, static_cast<DeviceInput<T> *>(this));

                    if (nullptr != input.second)
                    {
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
        virtual void remove_signal_source(BaseDevice *dev)
        {
            if (nullptr != dev && this != dev)
            {
                // try to dowcast to DeviceOutput pointer
                DeviceOutput<T> *out = dynamic_cast<DeviceOutput<T> *>(dev);

                if (out == input.second)
                {
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
        virtual void disconnect_signal_source(BaseDevice *dev)
        {
            if (nullptr != dev && this != dev)
            {
                // try to dowcast to DeviceOutput pointer
                DeviceOutput<T> *out = dynamic_cast<DeviceOutput<T> *>(dev);

                if (out == input.second)
                {
                    // set the external device pointer to nullptr, avoiding undesired deletion
                    input.second = nullptr;
                }
            }
        }

};

#endif
