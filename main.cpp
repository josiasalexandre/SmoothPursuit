#include <ImageMotionModel.hpp>
#include <sstream>

int main(int argc, char **argv)
{
    if (4 > argc)
    {
        std::cout << "Usage: program input_video frame_rate output_video groundtruth" << std::endl;;
        return -1;
    }

    std::string input_video(argv[1]);
    std::string output_video(argv[3]);
    std::string groundtruth(argv[4]);
    std::string statistics_file("statistics.csv");

    int frame_rate;
    std::istringstream iss(argv[2]);

    if (!(iss >> frame_rate))
    {
        std::cout << "Invalid frame rate, please provide a valid integer value." << std::endl;
        return -2;
    }


    ImageMotionModel imageMotionModel(input_video, frame_rate, groundtruth, output_video);
    imageMotionModel.Run(statistics_file);
    return 0;
}
