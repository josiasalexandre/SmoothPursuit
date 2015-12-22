#ifndef DSP_SYSTEM_H
#define DSP_SYSTEM_H

#include <ListT.hpp>

#include <BaseDevice.hpp>
#include <DRandomInputDevice.hpp>
#include <AddSignalDevice.hpp>
#include <InputVideoDevice.hpp>
#include <OpticalFlowCPUDevice.hpp>


class DSPSystem {

    // the devices lists
    List<BaseDevice *> input_devices, output_devices;
    List<BaseDevice *> iddle_devices, on_devices, running_devices;

    public:

        // the basic constructor
        DSPSystem () {}

        // the basic destructor
        ~DSPSystem () {

            BaseDevice *dev = nullptr;

            // destroy all input_devices
            while(!input_devices.empty()) {

                // get the first node element
                dev = input_devices.pop_front();

                if (nullptr != dev) {

                    // delete the node
                    delete dev;

                }

            }

            // destroy all the iddle_dsp
            while(!iddle_devices.empty()) {

                // get the first node element
                dev = iddle_devices.pop_front();

                if (nullptr != dev) {

                    // delete the node
                    delete dev;

                }

            }

            // destroy all the running_dsp
            while(!on_devices.empty()) {

                // get the first node element
                dev = on_devices.pop_front();

                if (nullptr != dev) {

                    // delete the node
                    delete dev;

                }

            }

            // destroy all the running_devices
            while(!running_devices.empty()) {

                // get the first node element
                dev = running_devices.pop_front();

                if (nullptr != dev) {

                    // delete the node
                    delete dev;

                }

            }

            // destroy all the output_devices
            while(!output_devices.empty()) {

                // get the first element node
                dev = output_devices.pop_front();

                if (nullptr != dev) {

                    // delete the node
                    delete dev;

                }

            }

            dev = nullptr;

        }

        void build_system() {

            /* TODO */
            // BaseDevice *video = new InputVideoDevice("/home/josias/IC/DSP/dsp/Examples/walk.avi", 10);
            BaseDevice *video = new InputVideoDevice(0, 10);

            BaseDevice *optical_flow = new OpticalFlowCPUDevice();
            optical_flow->update_status(Device_ON);

            video->connect(optical_flow);

            input_devices.push_back(video);
            on_devices.push_back(optical_flow);


        }

        // the main method
        void run() {

            int i = 0;

            BaseDevice *dev = nullptr;

            while(i < 300) {

                // run all the outputs
                while(output_devices.iterator()) {

                    dev = output_devices.current_element();

                    if (nullptr != dev) {
                        dev->run();
                    }

                }

                // run all the iddle_devices
                while(!iddle_devices.empty()) {

                    dev = iddle_devices.pop_front();

                    if (nullptr != dev) {

                        dev->run();

                        // append to running_devices
                        running_devices.push_back(dev);

                    }

                }

                // run all the on_devices
                while(!on_devices.empty()) {

                    dev = on_devices.pop_front();

                    if (nullptr != dev) {

                        dev->run();

                        // append to running_devices
                        running_devices.push_back(dev);

                    }

                }

                // run all the input_devices
                while(input_devices.iterator()) {

                    dev = input_devices.current_element();

                    if (nullptr != dev) {

                        dev->run();

                    }


                }

                // return the all the running devices to appropriate lists
                while (!running_devices.empty()) {

                    dev = running_devices.pop_front();

                    if (nullptr != dev) {

                        if (Device_ON == dev->get_device_status()) {

                            // append to on_devices
                            on_devices.push_back(dev);

                        } else {

                            // append to iddle_devices
                            iddle_devices.push_back(dev);

                        }

                    }

                }

                // delete the dev pointer
                dev = nullptr;

                i += 1;
            }

        };

};

#endif
