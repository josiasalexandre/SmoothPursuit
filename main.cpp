#include <ImageMotionModel.hpp>

int main(int argc, char **argv) {

    // build the ImageMotionModel
    // ImageMotionModel smooth_pursuit(40);
    ImageMotionModel smooth_pursuit("../Examples/ball.avi", 25);
    //ImageMotionModel smooth_pursuit("../Examples/ball_linear.avi", 25);
    //ImageMotionModel smooth_pursuit("../Examples/ball_step.avi", 25);
    // ImageMotionModel smooth_pursuit("../Examples/ball_step_x.avi", 25);
    // ImageMotionModel smooth_pursuit("../Examples/dot_step_x.avi", 25);

    // executes the simulation
    smooth_pursuit.run();

    return 0;

}
