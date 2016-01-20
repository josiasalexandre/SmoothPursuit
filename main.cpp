#include <ImageMotionModel.hpp>

int main(int argc, char **argv) {

    // build the ImageMotionModel
    // ImageMotionModel smooth_pursuit(25);
    //ImageMotionModel smooth_pursuit("../Examples/ball_step.avi", 25);
    ImageMotionModel smooth_pursuit("../Examples/ball_step_x.avi", 25);

    // executes the simulation
    smooth_pursuit.run();

    return 0;

}
