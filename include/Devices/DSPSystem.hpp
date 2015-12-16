#ifndef DSP_SYSTEM_H
#define DSP_SYSTEM_H

#include "ListT.hpp"
#include "BaseDevice.hpp"

template<typename T>
class DSPSystem {

    // the nodes list
    List<BaseDevice<T>*> idle, busy;

    public:

        // the basic constructor
        DSPSystem() {

        }

        // the main method
        void run() {};

};

#endif
