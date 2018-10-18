#ifndef DRANDOM_INPUT_DEVICE_H
#define DRANDOM_INPUT_DEVICE_H

#include <random>
#include <InputSignalDevice.hpp>

class DRandomInputDevice : virtual public InputSignalDevice<double>
{
    private:

        // the uniform real distribution
        std::uniform_real_distribution<double> distribution;

        // the generator engine
        std::default_random_engine generator;

    public:

        // basic constructor
        DRandomInputDevice(double left, double right) : generator(std::random_device {} ()), distribution(left, right) {}

        // the main device method
        virtual void run()
        {
            double rand = distribution(generator);

            // send the value to all external devices
            if (0 < DeviceOutput<double>::output.size())
            {
                // lock the output pointers
                DeviceOutput<double>::output_mutex.lock();

                int out_size = DeviceOutput<double>::output.size();

                for (int i = 0; i < out_size; i++)
                {
                    // send the value
                    DeviceOutput<double>::output[i].first->push(rand);
                }
                // unlock the output pointers
                DeviceOutput<double>::output_mutex.unlock();
            }
        };
};

#endif
