#ifndef RING_BUFFER_H
#define RING_BUFFER_H

template<typename T, unsigned int LENGTH>
class RingBuffer
{
    private:

        // our circular buffer
        T buffer[LENGTH];

        // the buffer max_length
        unsigned int length;

        // the head and tail pointers
        unsigned int offset;

        // the buffer acess mutex
        std::mutex buffer_mutex;
};

#endif
