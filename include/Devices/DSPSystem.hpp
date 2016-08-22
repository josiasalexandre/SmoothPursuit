#ifndef DSP_SYSTEM_H
#define DSP_SYSTEM_H

#include <thread>

#include <ListT.hpp>

#include <BaseDevice.hpp>
#include <DRandomInputDevice.hpp>
#include <AddSignalsDevice.hpp>
#include <SubtractVelocityDevice.hpp>
#include <LinearGainDevice.hpp>
#include <InputVideoDevice.hpp>
#include <OpticalFlowCPUDevice.hpp>
#include <FoveaDevice.hpp>
#include <LowPassExponencialDevice.hpp>
#include <FIRDevice.hpp>
#include <FirstDifferentiatorDevice.hpp>
#include <TrapzIntegratorDevice.hpp>
#include <DelayDevice.hpp>
#include <SimpleConnectionDevice.hpp>
#include <VelocityInterpolationDevice.hpp>
#include <SystemOutputDevice.hpp>
#include <SmoothGainDevice.hpp>
#include <ImpulseGainDevice.hpp>
#include <SaturationDevice.hpp>

class DSPSystem {

    private:

        // the system output
        std::vector<cv::Point2f> output;

        List<BaseDevice *> input_devices, output_devices;
        List<BaseDevice *> iddle_devices, running_devices, on_devices;

        // the first device input buffer
        CircularBuffer<cv::Point2f> *input_buffer;

        // build the entire system
        void build_system() {

            /* TODO */
            /*
             *  We should build some Parser and get the configuration from an external file (XML? Json?)
             *  THIS IS JUST A PROTOTYPE TO MAKE THE FIRST TESTS POSSIBLE
             *
             */

            // just a helper
            cv::Point2f null_value(0.0, 0.0);

            // the input
            LinearGainDevice *system_input = new LinearGainDevice(1.0);

            // get the input buffer
            input_buffer = system_input->get_buffer();

            // subtract velocities - just to simulate the step and sinusoid inputs
            // the fovea object already do the subtract
            // SubtractVelocityDevice<cv::Point2f> *subtract = new SubtractVelocityDevice<cv::Point2f>(cv::Point2f(0.0, 0.0));

            // get the input buffer
            // input_buffer = subtract->get_buffer(0);
            // subtract->add_signal_source(system_input, 0);

            // the delay device
            //DelayDevice *delay_1 = new DelayDevice(10);
            DelayDevice *delay_1 = new DelayDevice(72 - 25 - 1);
            // DelayDevice *delay_2 = new DelayDevice(77);

            // FIRST PATHWAY
            // the linear gain
            LinearGainDevice *linear_gain = new LinearGainDevice(6.7);

            // the low pass filter
            FIRDevice<61> *low_pass_1 = new FIRDevice<61>(1000, 5, LOW_PASS, HAMMING_WINDOW);

            // derivative
            FirstDifferentiatorDevice *differentiator = new FirstDifferentiatorDevice(null_value);

            // SECOND PATHWAY
            ImpulseGainDevice *impulse_gain = new ImpulseGainDevice(17500*0.85, 0.00015, 3000);

            // the low pass
            FIRDevice<61> *low_pass_2 = new FIRDevice<61>(1000, 8, LOW_PASS, HAMMING_WINDOW);

            // THIRD PATHWAY
            SmoothGainDevice *smooth_gain = new SmoothGainDevice(24*0.85, 0.08, 0.16, 500*0.25, 18.5*0.5);

            // the low pass
            FIRDevice<61> *low_pass_3 = new FIRDevice<61>(1000, 16, LOW_PASS, HAMMING_WINDOW);

            // summ all paths
            AddSignalsDevice<cv::Point2f, 3> *sum = new AddSignalsDevice<cv::Point2f, 3>(null_value);
            // AddSignalsDevice<cv::Point2f, 2> *sum = new AddSignalsDevice<cv::Point2f, 2>(cv::Point2f(0.0, 0.0));

            // integrator
            TrapzIntegratorDevice *integrator = new TrapzIntegratorDevice(null_value);

            // Just a first order IIR filter
            // the plant low pass filter
            FIRDevice<61> *plant = new FIRDevice<61>(1000, 22, LOW_PASS, HAMMING_WINDOW);
            // LowPassExponentialDevice *plant = new LowPassExponentialDevice(0.001, 0.004);

            // the feedback gain
            // LinearGainDevice *feedback_gain = new LinearGainDevice(1);

            // the system output
            SystemOutputDevice<cv::Point2f> *system_output = new SystemOutputDevice<cv::Point2f>(&output);

            // CONNECT ALL DEVICES
            delay_1->add_signal_source(system_input);
            // delay_1->add_signal_source(subtract);
            // delay_2->add_signal_source(subtract);


            // connect the subtract output to linear gain input
            //linear_gain->add_signal_source(subtract);
            linear_gain->add_signal_source(delay_1);

            // connect the low_pass_1 to the linear gain
            low_pass_1->add_signal_source(linear_gain);
            //low_pass_1->add_signal_source(differentiator_2);

            // connect the delay to the differentiator device
            differentiator->add_signal_source(delay_1);
            // differentiator->add_signal_source(delay_2);
            // differentiator->add_signal_source(linear_gain);

            // connect the impulse gain
            impulse_gain->add_signal_source(differentiator);

            // connect the second low pass
            low_pass_2->add_signal_source(impulse_gain);

            // connect the smooth gain
            smooth_gain->add_signal_source(differentiator);
            // smooth_gain->add_signal_source(linear_gain);

            // connect the third low pass
            low_pass_3->add_signal_source(smooth_gain);

            // connect the integrator
            //integrator->add_signal_source(low_pass_3);

            // summ all signals
            sum->add_signal_source(low_pass_1);
            sum->add_signal_source(low_pass_2);
            sum->add_signal_source(low_pass_3);

            // connect the integrator to the sum
            integrator->add_signal_source(sum);

            // connect the integrator to the plant device
            plant->add_signal_source(integrator);

            // control the feedback gain
            // feedback_gain->add_signal_source(plant);

            // the feedback connection FOVEA
            // subtract->add_signal_source(plant, 1);

            // connect the plant to output
            system_output->add_signal_source(plant);

            // SAVE THE DEVICES
            input_devices.push_back(system_input);
            // on_devices.push_back(subtract);
            on_devices.push_back(delay_1);
            // on_devices.push_back(delay_2);
            on_devices.push_back(linear_gain);
            on_devices.push_back(low_pass_1);
            on_devices.push_back(differentiator);
            on_devices.push_back(impulse_gain);
            on_devices.push_back(low_pass_2);
            on_devices.push_back(smooth_gain);
            on_devices.push_back(low_pass_3);
            on_devices.push_back(sum);
            on_devices.push_back(integrator);
            on_devices.push_back(plant);
            // on_devices.push_back(feedback_gain);
            output_devices.push_back(system_output);

        }

    public:

        // the basic constructor
        DSPSystem () : output(0), input_buffer(nullptr) {

            // build the entire system
            build_system();

        }

        // the basic destructor
        ~DSPSystem () {

        }


        // reset the entire system
        void reset() {

            BaseDevice *dev;

            // run all the outputs
            while(output_devices.iterator()) {

                dev = output_devices.current_element();

                if (nullptr != dev) {
                    dev->reset();
                }

            }

            // run all the iddle_devices
            while(iddle_devices.iterator()) {

                dev = iddle_devices.current_element();

                if (nullptr != dev) {
                    dev->reset();
                }

            }

            // run all the on_devices
            while(on_devices.iterator()) {

                dev = on_devices.current_element();

                if (nullptr != dev) {
                    dev->reset();
                }

            }

            // run all the input_devices
            while(input_devices.iterator()) {

                dev = input_devices.current_element();

                if (nullptr != dev) {

                    dev->reset();

                }


            }


        }

        // remove all devices
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
        std::vector<cv::Point2f> run(std::vector<cv::Point2f> &input) {

            // get the input size
            int input_size = input.size();

            // cleat the output vector
            output.clear();

            BaseDevice *dev = nullptr;

            // testing, the limit should be better than this
            for(int i = 0; i < input_size; i++) {

                // send the input to the first device in the list
                input_buffer->push(input[i]);

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

                dev = nullptr;

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

                // reset the dev pointer
                dev = nullptr;

            }

            // return the output vector
            return output;

        };

        // get the input buffer
        CircularBuffer<cv::Point2f>* get_input_buffer() {

            return input_buffer;

        }

};

#endif
