#include <DSPSystem.hpp>

int main(int argc, char **argv) {

    DSPSystem system;

    system.build_system();

    cv::namedWindow("frame", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("fovea", cv::WINDOW_AUTOSIZE);

    system.run();

    system.disconnect_all();

    return 0;

}
