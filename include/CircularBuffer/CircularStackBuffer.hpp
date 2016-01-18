#ifndef CIRCULAR_STACK_BUFFER_H
#define CIRCULAR_STACK_BUFFER_H

#include <new>
#include <mutex>

#define MIN_BUFFER_SIZE 64
#define MAX_BUFFER_SIZE 1024

template<typename T>
class CircularStackBuffer {

    private:

        // our circular buffer
        T *buffer;

        // the buffer max_length
        unsigned int max_length;

        // the head and tail pointers
        unsigned int offset;

        // the supplied null value reference
        T null_value;

        // the buffer acess mutex
        std::mutex buffer_mutex;

    public:

        // basic constructor withoud the null value param
        CircularStackBuffer() : max_length(MIN_BUFFER_SIZE), head(0), tail(0) {

            // allocates the buffer
            buffer = new T[max_length]();

            if (nullptr == buffer) {

                throw std::bad_alloc();

            }

        }

        // basic constructor
        CircularStackBuffer(T v_null) : max_length(MIN_BUFFER_SIZE), head(0), tail(0), null_value(v_null) {

            // allocates the buffer
            buffer = new T[max_length]();

            if (nullptr == buffer) {

                throw std::bad_alloc();

            }

        }

        // another basic constructor
        CircularStackBuffer(unsigned int buffer_max_length, T v_null) : max_length(buffer_max_length), head(0), tail(0), null_value(v_null) {

            // verify buffer max_length
            if (MIN_BUFFER_SIZE > max_length) {

                max_length = MIN_BUFFER_SIZE;

            } else if (MAX_BUFFER_SIZE < max_length) {

                max_length = MAX_BUFFER_SIZE;

            }

            // allocates the buffer
            buffer = new T[max_length]();
            if (nullptr == buffer) {
                throw std::bad_alloc();
            }

        }

        // copy constructor
        CircularStackBuffer(const CircularStackBuffer<T> &cb) : buffer(nullptr) {

            // lock the external circular buffer
            cb.buffer_mutex.lock();

            // copy the values
            max_length = cb.max_length;
            head = tail = 0;
            null_value = cb.null_value;

             // allocates the buffer
            buffer = new T[max_length]();

            if (nullptr == buffer) {

                throw std::bad_alloc();

            }

            // unlock the external circular buffer
            cb.buffer_mutex.unlock();

        }

        // basic destructor
        ~CircularStackBuffer() {

            if (nullptr != buffer) {

                delete [] buffer;

            }

        }

        // push new element
        void push(T e) {

            // lock the buffer
            buffer_mutex.lock();

            // verify if we can push something at the next position
            if ((head + 1) == tail || (head + 1 == max_length && 0 == tail)) {

                // insert the new element at the head position but dont increment the head
                buffer[head] = e;

            } else {

                // insert the new element at the head position and increments the head index
                buffer[head++] = e;

                // verify the limit
                if (head >= max_length) {

                    //reset
                    head = 0;

                }

            }

            // unlock the buffer
            buffer_mutex.unlock();

        }

        // pop new element
        T pop() {

            T e = null_value;

            // lock the buffer
            buffer_mutex.lock();

            // is empty?
            if (tail != head) {

                // get the element at the tail position
                e = buffer[tail++];

                // verify the limit
                if (max_length <= tail) {

                    //reset
                    tail = 0;
                }

            }

            // unlock the buffer
            buffer_mutex.unlock();

            return e;

        }

        // get the current element (tail index)
        T get_current() {

            T e = null_value;

            // lock the buffer
            buffer_mutex.lock();


            // is empty?
            if (tail != head) {

                // get the element at the tail position
                e = buffer[tail];

            }

            // unlock the buffer
            buffer_mutex.unlock();

            return e;

        }

        // the circular buffer max_length
        unsigned int get_length() {

            return max_length;

        }

        // is empty?
        bool empty() {

            bool status;

            // lock the buffer
            buffer_mutex.lock();

            status = (head == tail);

            // unlock the buffer
            buffer_mutex.unlock();

            // if head and tail are equal then this buffer is empty
            return status;

        }

        // is this ring buffer really full?
        bool full() {

            bool status;

            // lock the buffer
            buffer_mutex.lock();

            status = ((head + 1) == tail || (head + 1 == max_length && 0 == tail));

            // unlock the buffer
            buffer_mutex.unlock();

            // if head and tail are equal then this buffer is empty
            return status;

        }

        // set the current null value
        void set_null_value(T v_null) {

            // update the null_value
            null_value = v_null;

        }

        // get the current null value
        T get_null_value() {

            return null_value;

        }

        // get the queue size
        unsigned int get_size() {

            return std::abs(head - tail);

        }

};

#endif
