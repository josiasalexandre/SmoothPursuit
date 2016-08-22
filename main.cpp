#include <ImageMotionModel.hpp>

int main(int argc, char **argv) {

    // the first experiment
    // std::string input_video = "/home/josias/Projects/dsp/Examples/Lemming/Lemming0001-1336.avi";
    // int frame_rate = 25;
    // std::string groundtruth = "/home/josias/Projects/dsp/Examples/Lemming/groundtruth_rect.txt";
    // std::string output_video = "/home/josias/Projects/dsp/Examples/Lemming/LemmingTracking.avi";

    std::string input_video = "/home/josias/Projects/dsp/Examples/Walking/walking0001-0412.avi";
    int frame_rate = 25;
    std::string groundtruth = "/home/josias/Projects/dsp/Examples/Walking/groundtruth_rect.txt";
    std::string output_video = "/home/josias/Projects/dsp/Examples/Walking/WalkingTracking.avi";

    // build the ImageMotionModel
    // ImageMotionModel smooth_pursuit(25);
    ImageMotionModel smooth_pursuit(input_video, frame_rate, groundtruth, output_video);

    // executes the simulation
    smooth_pursuit.Run();

    return 0;

}
