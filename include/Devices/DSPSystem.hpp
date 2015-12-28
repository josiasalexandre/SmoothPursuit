#ifndef DSP_SYSTEM_H
#define DSP_SYSTEM_H

#include <ListT.hpp>

#include <BaseDevice.hpp>
#include <DRandomInputDevice.hpp>
#include <AddSignalsDevice.hpp>
#include <InputVideoDevice.hpp>
#include <OpticalFlowCPUDevice.hpp>
#include <RetinaDevice.hpp>
#include <LowPassExponencialDevice.hpp>

class DSPSystem {

    // the devices lists
    List<BaseDevice *> input_devices, output_devices;
    List<BaseDevice *> iddle_devices, on_devices, running_devices;

    public:

        // the basic constructor
        DSPSystem () {}

        // the basic destructor
        ~DSPSystem () {


        }

        void build_system() {

            /* TODO */
            /*
             *  We should build some Parser and get the configuration form an external file (XML? Json?)
             *
             */
            BaseDevice *video = new InputVideoDevice("../Examples/ball.avi", 30);
            //BaseDevice *video = new InputVideoDevice("../Examples/walk.avi", 10);
            // BaseDevice *video = new InputVideoDevice(0, 10);

            cv::Mat empty_matrix;

            OpticalFlowCPUDevice *optical_flow = new OpticalFlowCPUDevice(empty_matrix.clone());
            optical_flow->update_status(Device_ON);

            RetinaDevice *retina = new RetinaDevice();

            LowPassExponencialDevice *lpf = new LowPassExponencialDevice();

            retina->add_signal_source(video);
            optical_flow->add_signal_source(retina);
            lpf->add_signal_source(optical_flow);
            retina->add_signal_source(lpf);

            input_devices.push_back(video);
            on_devices.push_back(retina);
            on_devices.push_back(optical_flow);
            on_devices.push_back(lpf);

        }

        void disconnect_all() {

            BaseDevice *dev;

            // disconnects all input_devices
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

        }

        // the main method
        void run() {

            int i = 0;

            BaseDevice *dev = nullptr;

            while(i < 524) {

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

                        // append to running_devices
                        running_devices.push_back(dev);

                        dev->run();

                    }

                }

                // run all the on_devices
                while(!on_devices.empty()) {

                    dev = on_devices.pop_front();

                    if (nullptr != dev) {

                        // append to running_devices
                        running_devices.push_back(dev);

                        dev->run();

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
