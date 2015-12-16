#include <iostream>

#include <MatrixT.hpp>
#include <CircularBuffer.hpp>
#include <SingleInputDevice.hpp>
#include <MultInputDevice.hpp>

int main(int argc, char **argv) {

    SingleInputDevice<double> sid1, sid2, sid3, sid5;
    MultInputDevice<double, 4> mid1;
    SingleInputDevice<double> *sid4 = new SingleInputDevice<double>();

    mid1.add_signal_source(&sid1);
    mid1.add_signal_source(&sid2);
    mid1.add_signal_source(&sid3);
    mid1.add_signal_source(sid4);


    sid1.add_signal_source(&sid2);
    sid4->add_signal_source(&sid1);
    sid3.add_signal_source(sid4);

    sid3.connect(&sid5);
    sid1.remove_signal_source(&sid3);
    delete sid4;
    sid1.remove_signal_source(&sid2);

//     DeviceInput<double> *d = static_cast<DeviceInput<double> *>(&sid1);
//
//     DeviceOutput<double> *out = static_cast<DeviceOutput<double> *>(&sid2);
//
//     out->add_output(d);
//
//     sid1.add_signal_source(&sid2);
//     sid1.remove_signal_source(&sid2);
//     sid1.add_signal_source(&sid3);
//
//     out->add_output(d);

    CircularBuffer<double> ring(MIN_BUFFER_SIZE + 10);

    for (int j = 0; j < MIN_BUFFER_SIZE + 11; j++) {
        std::cout << ring.pop() - 1 << " ";
    }

    for (int i = 0; i < MIN_BUFFER_SIZE + 10; i++) {
        ring.push((double) i+1);
        std::cout << i+1 << " ";
    }

    std::cout << std::endl << "next:" << std::endl;

    for (int j = 0; j < MIN_BUFFER_SIZE + 11; j++) {
        std::cout << ring.pop() << " ";
    }

    Matrix<double> m1(10);
    Matrix<int> m2(10);
    Matrix<int> m5(m2);
    Matrix<unsigned long int> m3(11);
    Matrix<float> m4(12);

    Matrix<int> m6;

    m6 = m2*m2;
    m6.mult(m6);
    m6.set(0,1, 10);
    // m6.transpose().show();

    m6.add(m6);

    Matrix<int> k = m6.transpose();

    k.selfTranspose();

    // k.show();

    // std::cout << m1.at(0,0) << std::endl;
    // std::cout << "Hello, world!" << std::endl;

    if (m6.isEmpty() || m6.isSquareMatrix() || m6.sameDimensions(m2)) {
        return 1;
    }
    return 0;
}
