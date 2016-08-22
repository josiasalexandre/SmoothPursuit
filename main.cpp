#include <ImageMotionModel.hpp>

int main(int argc, char **argv) {

    // the first experiment
    std::string input_video("/home/josias/Projects/dsp/Examples/Lemming/Lemming.avi");
    int frame_rate = 25;
    std::string groundtruth("/home/josias/Projects/dsp/Examples/Lemming/groundtruth_rect.txt");
    std::string output_video("/home/josias/Projects/dsp/Examples/Lemming/LemmingTracking.avi");
    std::string statistics_file("/home/josias/Projects/dsp/Examples/Lemming/statistics.csv");

    // build the ImageMotionModel
    ImageMotionModel lemming(input_video, frame_rate, groundtruth, output_video);

    // run
    lemming.Run(statistics_file);

    // the second experiment
    // std::string input_video = "/home/josias/Projects/dsp/Examples/Walking/Walking.avi";
    // int frame_rate = 25;
    // std::string groundtruth = "/home/josias/Projects/dsp/Examples/Walking/groundtruth_rect.txt";
    // std::string output_video = "/home/josias/Projects/dsp/Examples/Walking/WalkingTracking.avi";
    // std::string statistics_file = "/home/josias/Projects/dsp/Examples/Walking/statistics.csv";

    // build the ImageMotionModel
    // ImageMotionModel walking(input_video, frame_rate, groundtruth, output_video);

    // executes the simulation
    // walking.Run(statistics_file);

    // the third experiment
    // std::string input_video = "/home/josias/Projects/dsp/Examples/Sylvester/Sylvester.avi";
    // int frame_rate = 25;
    // std::string groundtruth = "/home/josias/Projects/dsp/Examples/Sylvester/groundtruth_rect.txt";
    // std::string output_video = "/home/josias/Projects/dsp/Examples/Sylvester/SylvesterTracking.avi";
    // std::string statistics_file = "/home/josias/Projects/dsp/Examples/Sylvester/statistics.csv";

    // build the ImageMotionModel
    // ImageMotionModel sylvester(input_video, frame_rate, groundtruth, output_video);

    // executes the simulation
    // sylvester.Run(statistics_file);

    // the fourth experiment
    // std::string input_video = "/home/josias/Projects/dsp/Examples/Car4/Car4.avi";
    // int frame_rate = 25;
    // std::string groundtruth = "/home/josias/Projects/dsp/Examples/Car4/groundtruth_rect.txt";
    // std::string output_video = "/home/josias/Projects/dsp/Examples/Car4/Car4Tracking.avi";
    // std::string statistics_file = "/home/josias/Projects/dsp/Examples/Car4/statistics.csv";

    // build the ImageMotionModel
    // ImageMotionModel car4(input_video, frame_rate, groundtruth, output_video);

    // executes the simulation
    // car4.Run(statistics_file);

    // the fifth experiment
    // std::string input_video = "/home/josias/Projects/dsp/Examples/CarScale/CarScale.avi";
    // int frame_rate = 25;
    // std::string groundtruth = "/home/josias/Projects/dsp/Examples/CarScale/groundtruth_rect.txt";
    // std::string output_video = "/home/josias/Projects/dsp/Examples/CarScale/CarScaleTracking.avi";
    // std::string statistics_file = "/home/josias/Projects/dsp/Examples/CarScale/statistics.csv";

    // build the ImageMotionModel
    // ImageMotionModel carscale(input_video, frame_rate, groundtruth, output_video);

    // executes the simulation
    // carscale.Run(statistics_file);

    // the sixth experiment
    // std::string input_video = "/home/josias/Projects/dsp/Examples/Human/Human.avi";
    // int frame_rate = 25;
    // std::string groundtruth = "/home/josias/Projects/dsp/Examples/Human/groundtruth_rect.txt";
    // std::string output_video = "/home/josias/Projects/dsp/Examples/Human/HumanTracking.avi";
    // std::string statistics_file = "/home/josias/Projects/dsp/Examples/Human/statistics.csv";

    // build the ImageMotionModel
    // ImageMotionModel human(input_video, frame_rate, groundtruth, output_video);

    // executes the simulation
    // human.Run(statistics_file);

    // the sixth experiment
    // std::string input_video = "/home/josias/Projects/dsp/Examples/Dog1/Dog1.avi";
    // int frame_rate = 25;
    // std::string groundtruth = "/home/josias/Projects/dsp/Examples/Dog1/groundtruth_rect.txt";
    // std::string output_video = "/home/josias/Projects/dsp/Examples/Dog1/Dog1Tracking.avi";
    // std::string statistics_file = "/home/josias/Projects/dsp/Examples/Dog1/statistics.csv";

    // build the ImageMotionModel
    // ImageMotionModel dog1(input_video, frame_rate, groundtruth, output_video);

    // executes the simulation
    // dog1.Run(statistics_file);

    // the seventh experiment
    // std::string input_video = "/home/josias/Projects/dsp/Examples/Panda/Panda.avi";
    // int frame_rate = 25;
    // std::string groundtruth = "/home/josias/Projects/dsp/Examples/Panda/groundtruth_rect.txt";
    // std::string output_video = "/home/josias/Projects/dsp/Examples/Panda/PandaTracking.avi";
    // std::string statistics_file = "/home/josias/Projects/dsp/Examples/Panda/statistics.csv";

    // build the ImageMotionModel
    // ImageMotionModel panda(input_video, frame_rate, groundtruth, output_video);

    // executes the simulation
    // panda.Run(statistics_file);

    // the eigth experiment
    // std::string input_video = "/home/josias/Projects/dsp/Examples/BlurFace/BlurFace.avi";
    // int frame_rate = 25;
    // std::string groundtruth = "/home/josias/Projects/dsp/Examples/BlurFace/groundtruth_rect.txt";
    // std::string output_video = "/home/josias/Projects/dsp/Examples/BlurFace/BlurFaceTracking.avi";
    // std::string statistics_file = "/home/josias/Projects/dsp/Examples/BlurFace/statistics.csv";

    // build the ImageMotionModel
    // ImageMotionModel blurface(input_video, frame_rate, groundtruth, output_video);

    // executes the simulation
    // blurface.Run(statistics_file);

    // the nineth experiment
    // std::string input_video = "/home/josias/Projects/dsp/Examples/Girl/Girl.avi";
    // int frame_rate = 25;
    // std::string groundtruth = "/home/josias/Projects/dsp/Examples/Girl/groundtruth_rect.txt";
    // std::string output_video = "/home/josias/Projects/dsp/Examples/Girl/GirlTracking.avi";
    // std::string statistics_file = "/home/josias/Projects/dsp/Examples/Girl/statistics.csv";

    // build the ImageMotionModel
    // ImageMotionModel girl(input_video, frame_rate, groundtruth, output_video);

    // executes the simulation
    // girl.Run(statistics_file);

    // the tenth experiment
    // std::string input_video = "/home/josias/Projects/dsp/Examples/Gym/Gym.avi";
    // int frame_rate = 25;
    // std::string groundtruth = "/home/josias/Projects/dsp/Examples/Gym/groundtruth_rect.txt";
    // std::string output_video = "/home/josias/Projects/dsp/Examples/Gym/GymTracking.avi";
    // std::string statistics_file = "/home/josias/Projects/dsp/Examples/Gym/statistics.csv";

    // build the ImageMotionModel
    // ImageMotionModel gym(input_video, frame_rate, groundtruth, output_video);

    // executes the simulation
    // gym.Run(statistics_file);

    return 0;

}
