#include <ImageMotionModel.hpp>

int main(int argc, char **argv) {

    ImageMotionModel smooth_pursuit(25);
//     ImageMotionModel smooth_pursuit("../../dsp/Examples/ball.avi", 25);

    smooth_pursuit.run();
    return 0;

}
