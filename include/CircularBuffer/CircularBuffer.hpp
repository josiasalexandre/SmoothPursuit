#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#define MIN_BUFFER_SIZE 64
#define MAX_BUFFER_SIZE 1024

template<typename T>
class CircularBuffer {

    private:

        // our circular buffer
        T *buffer;

        // the buffer size
        unsigned int size;

        // the head and tail pointers
        unsigned int head, tail;

        // the supplied null value reference
        T null_value;

    public:

        // basic constructor withoud the null value param
        CircularBuffer() : size(MIN_BUFFER_SIZE), head(0), tail(0) {

            // allocates the buffer
            buffer = new T[size]();

            if (nullptr == buffer) {

                throw std::bad_alloc();

            }

        }

        // basic constructor
        CircularBuffer(T v_null) : size(MIN_BUFFER_SIZE), head(0), tail(0), null_value(v_null) {

            // allocates the buffer
            buffer = new T[size]();

            if (nullptr == buffer) {

                throw std::bad_alloc();

            }

        }

        // another basic constructor
        CircularBuffer(unsigned int buffer_size, T v_null) : size(buffer_size), head(0), tail(0), null_value(v_null) {

            // verify buffer size
            if (MIN_BUFFER_SIZE > size) {

                size = MIN_BUFFER_SIZE;

            } else if (MAX_BUFFER_SIZE < size) {

                size = MAX_BUFFER_SIZE;

            }

            // allocates the buffer
            buffer = new T[size]();
            if (nullptr == buffer) {
                throw std::bad_alloc();
            }

        }

        // basic destructor
        ~CircularBuffer() {

            if (nullptr != buffer) {

                delete [] buffer;

            }

        }

        // push new element
        void push(T e) {

            if ((head + 1) == tail || (head + 1 == size && 0 == tail)) {

                // insert the new element at the head position but dont increment the head
                buffer[head] = e;

            } else {

                // insert the new element at the head position
                buffer[head++] = e;

                // verify the limit
                if (head >= size) {

                    //reset
                    head = 0;

                }

            }

        }

        // pop new element
        T pop() {

            // is empty?
            if (tail == head) {

                return null_value;

            }

            // get the element at the tail position
            T e = buffer[tail++];

            // verify the limit
            if (size <= tail) {

                //reset
                tail = 0;
            }

            //
            return e;

        }

        // the circular buffer size
        unsigned int getSize() {

            return size;

        }

        // is empty?
        bool empty() {

            // if head and tail are equal then this buffer is empty
            return head == tail;

        }

        // is this ring buffer really full?
        bool full() {

            return ((head + 1) == tail || (head + 1 == size && 0 == tail));

        }

        // set the current null value
        void set_null_value(T v_null) {

            // update the null_value
            null_value = v_null;

        }
};

#endif
