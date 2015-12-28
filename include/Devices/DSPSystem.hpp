#ifndef DSP_SYSTEM_H
#define DSP_SYSTEM_H

#include <ListT.hpp>

#include <BaseDevice.hpp>
#include <DRandomInputDevice.hpp>
#include <AddSignalsDevice.hpp>
#include <InputVideoDevice.hpp>
#include <OpticalFlowCPUDevice.hpp>
#include <FoveaDevice.hpp>
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
             *  We should build some Parser and get the configuration from an external file (XML? Json?)
             *  THIS IS JUST A PROTOTYPE TO MAKE THE FIRST TESTS POSSIBLE
             *  THE NEXT STEP: IMPLEMENT THE MISSING DEVICES AND BUILD THE SYSTEM FOLLOWING THE SMOOTH PURSUIT MODELS
             *
             */
            BaseDevice *video = new InputVideoDevice("../Examples/ball.avi", 30);
            //BaseDevice *video = new InputVideoDevice("../Examples/walk.avi", 10);
            // BaseDevice *video = new InputVideoDevice(0, 10);

            // just to provide an empty Matrix as the null value
            cv::Mat empty_matrix;

            // build the optical flow device
            OpticalFlowCPUDevice *optical_flow = new OpticalFlowCPUDevice(empty_matrix.clone());
            // set the device ON
            optical_flow->update_status(Device_ON);

            // build a FoveaDevice: crop the ROI and send to the external devices
            FoveaDevice *fovea = new FoveaDevice();

            // build a LowPassExponencial filter
            LowPassExponencialDevice *lpf = new LowPassExponencialDevice();

            // connecting the nodes
            // the fovea receives the the frame from the video input device
            fovea->add_signal_source(video);

            // the optical flow device receives the fovea from the fovea device
            optical_flow->add_signal_source(fovea);

            // the Low Pass exponencial filter receives the optical flow
            lpf->add_signal_source(optical_flow);

            // the fovea receives the velocity command from the low pass filter
            fovea->add_signal_source(lpf);

            // includes all devices to the system task manager
            input_devices.push_back(video);
            on_devices.push_back(fovea);
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

            // testing, the limit should be better than this
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
