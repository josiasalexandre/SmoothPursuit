#include <ImageMotionModel.hpp>

int main(int argc, char **argv) {

    std::string filename;
    if (1 < argc) {

        filename = std::string(argv[1]);

    }

    if (filename.empty()) {

        // filename = "../Examples/walk.avi";
        // filename = "../Examples/ball.avi";
        // filename = "../Examples/ball_linear.avi";
        // filename = "../Examples/ball_step.avi";
        // filename = "../Examples/ball_step_x.avi";
        // filename = "../Examples/dot_step_x.avi";
        // filename = "../Examples/dot_step_x_texture.avi";
        // filename = "../Examples/ball_bg_texture_1.avi";
        // filename = "../Examples/ball_bg_texture_2.avi";
        // filename = "../Examples/ball_bg_texture_3.avi";
        filename = "../Examples/ball_bg_texture_3.avi";
        // filename = "../Examples/walk.avi";
        // filename = "../Examples/walk.avi";
        // filename = "../Examples/walk.avi";
        // filename = "../Examples/walk.avi";
    }

    // build the ImageMotionModel
    // ImageMotionModel smooth_pursuit(25);
    ImageMotionModel smooth_pursuit(filename, 25);

    // executes the simulation
    smooth_pursuit.run();

    return 0;

}
