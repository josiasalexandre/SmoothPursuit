#include <iostream>

#include <MatrixT.hpp>
#include <CircularBuffer.hpp>
#include <ListT.hpp>

#include <DRandomInputDevice.hpp>
#include <AddSignalDevice.hpp>
#include <DSPSystem.hpp>

#include <OpticalFlowCPUDevice.hpp>

int main(int argc, char **argv) {

    DSPSystem system;

    system.build_system();

    cv::namedWindow("frame", cv::WINDOW_AUTOSIZE);
    system.run();

    CircularBuffer<double> ring(MIN_BUFFER_SIZE + 10, 0.0);

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



    std::cout << std::endl << "next 2:" << std::endl;
    if (m6.isEmpty() || m6.isSquareMatrix() || m6.sameDimensions(m2)) {
        return 1;
    }
    return 0;

}
