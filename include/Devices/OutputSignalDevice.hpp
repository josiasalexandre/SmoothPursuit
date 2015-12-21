#ifndef OUTPUT_SIGNAL_DEVICE_H
#define OUTPUT_SIGNAL_DEVICE_H

#include <DeviceInput.hpp>

template<typename T>
class OutputSignalDevice : virtual public DeviceInput<T> {};

#endif
