#ifndef SIGNAL_GENERATOR_DEVICE_H
#define SIGNAL_GENERATOR_DEVICE_H

#include "BaseDevice.hpp"
#include "DeviceOutput.hpp"
#include "DeviceInput.hpp"

template<typename T>
class SignalGenerator : virtual public BaseDevice<T>, virtual public DeviceOutput<T> {};

#endif
