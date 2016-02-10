#include <ImageMotionModel.hpp>

int main(int argc, char **argv) {

    // build the ImageMotionModel
    // ImageMotionModel smooth_pursuit(25);
    // ImageMotionModel smooth_pursuit("../Examples/walk.avi", 10);
    // ImageMotionModel smooth_pursuit("../Examples/ball_bg_texture_1.avi", 25);
    // ImageMotionModel smooth_pursuit("../Examples/ball_bg_texture_2.avi", 25);
    ImageMotionModel smooth_pursuit("../Examples/ball_bg_texture_3.avi", 25);
    // ImageMotionModel smooth_pursuit("../Examples/ball.avi", 25);
    // ImageMotionModel smooth_pursuit("../Examples/ball_linear.avi", 25);
    // ImageMotionModel smooth_pursuit("../Examples/ball_step.avi", 25);
    // ImageMotionModel smooth_pursuit("../Examples/ball_step_x.avi", 25);
    // ImageMotionModel smooth_pursuit("../Examples/dot_step_x.avi", 25);
    // ImageMotionModel smooth_pursuit("../Examples/dot_step_x_texture.avi", 25);

    // executes the simulation
    smooth_pursuit.run();

    return 0;

}
