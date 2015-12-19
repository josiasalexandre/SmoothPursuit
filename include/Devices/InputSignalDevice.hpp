#ifndef INPUT_SIGNAL_DEVICE_H
#define INPUT_SIGNAL_DEVICE_H

#include "DeviceOutput.hpp"

template<typename T>
class InputSignalDevice : virtual public DeviceOutput<T> {};

#endif
