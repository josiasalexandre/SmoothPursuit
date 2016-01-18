#ifndef DSP_SYSTEM_H
#define DSP_SYSTEM_H

#include <thread>

#include <ListT.hpp>

#include <BaseDevice.hpp>
#include <DRandomInputDevice.hpp>
#include <AddSignalsDevice.hpp>
#include <SubtractVelocityDevice.hpp>
#include <InputVideoDevice.hpp>
#include <OpticalFlowCPUDevice.hpp>
#include <FoveaDevice.hpp>
#include <LowPassExponencialDevice.hpp>
#include <FirstDifferentiatorDevice.hpp>
#include <TrapzIntegratorDevice.hpp>
#include <DelayDevice.hpp>
#include <SimpleConnectionDevice.hpp>
#include <VelocityInterpolationDevice.hpp>

template<typename T>
class DSPSystem {

    private:

        // the system output
        CircularBuffer<T> output;

        List<BaseDevice *> input_devices, output_devices;
        List<BaseDevice *> iddle_devices, running_devices, on_devices;

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
             *
             */
            BaseDevice *video = new InputVideoDevice("../Examples/ball.avi", 25);
            // BaseDevice *video = new InputVideoDevice("../Examples/walk.avi", 10);
            //BaseDevice *video = new InputVideoDevice(0, 25);

            // the fovea device
            FoveaDevice *fovea = new FoveaDevice();

            // the optical flow device
            OpticalFlowCPUDevice *optical_flow = new OpticalFlowCPUDevice(cv::Mat());

            // the interpolation device
            VelocityInterpolationDevice *interpolation = new VelocityInterpolationDevice(LINEAR_INTERPOLATION);

            // the subtract device
            SubtractVelocityDevice<cv::Point2f> *subtract = new SubtractVelocityDevice<cv::Point2f>(cv::Point2f(0.0, 0.0));

            /*

            // the add signal device
            AddSignalsDevice<cv::Point2f, 2> *add_velocities = new AddSignalsDevice<cv::Point2f, 2>(cv::Point2f(0.0, 0.0));

            // the delay device 65 ms
            DelayDevice *delay = new DelayDevice(65);

            */
            // CONNECT ALL DEVICES
            // the input video to the fovea device
            fovea->add_signal_source(video);

            // the the fovea to the optical flow
            optical_flow->add_signal_source(fovea);

            fovea->add_signal_source(optical_flow);

            // the optical flow to the interpolation device
            interpolation->add_signal_source(optical_flow);

            // SimpleConnectionDevice
            //SimpleConnectionDevice<>

            // the interpolation to the subtract device
            subtract->add_signal_source(interpolation);

            /*


            // THIS IS THE FIRST CONNECTION
            add_velocities->add_signal_source(interpolation);

            // the add_velocities to the delay device
            delay->add_signal_source(add_velocities);
            */

            //
            input_devices.push_back(video);

            running_devices.push_back(fovea);
            running_devices.push_back(optical_flow);
            running_devices.push_back(interpolation);


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

                cv::waitKey(25);

            }

        };

        // the start method
        void start() {

            std::thread dsp(&DSPSystem::run, this);

            dsp.detach();

        }
};

#endif
