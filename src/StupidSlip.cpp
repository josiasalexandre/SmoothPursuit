#include <StupidSlip.hpp>

// basic constructor
StupidSlip::StupidSlip(unsigned int value) : threshold(value) {}

cv::Point2f StupidSlip::displacement(cv::Point2i center, cv::Mat frame) {

    cv::Point2i new_value(0.0, 0.0), disp(0.0, 0.0);

    int k = 1;

    unsigned int red, green, blue;

    if (!frame.empty()) {

        for (int i = 0; i < frame.rows; i++) {

            for (int j = 0; j < frame.cols; j++) {

                blue = frame.at<cv::Vec3b>(i, j).val[0];
                green = frame.at<cv::Vec3b>(i, j).val[1];
                red = frame.at<cv::Vec3b>(i, j).val[2];

                if (threshold < red && threshold > green && threshold < blue) {

                    new_value.x += j;
                    new_value.y += i;
                    k++;

                }

            }

        }

        new_value /= k;

        disp = new_value - center;
        disp.x = std::floor(disp.x + 0.6);
        disp.y = std::floor(disp.y + 0.6);


    }

    return disp;

}
