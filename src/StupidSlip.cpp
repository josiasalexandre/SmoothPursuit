#include <StupidSlip.hpp>

// basic constructor
StupidSlip::StupidSlip(unsigned int value) : threshold(value) {}

cv::Point2i StupidSlip::displacement(cv::Point2i center, cv::Mat frame) {

    // convert to gray
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    cv::Point2i disp(0.0, 0.0);

    cv::Vec3b *pixel;

    int k = 1;

    if (!gray.empty()) {

        for (int i = 0; i < gray.rows; i++) {

            for (int j = 0; j < gray.cols; j++) {

                pixel = &gray.at<cv::Vec3b>(i, j);

                if (threshold >= (unsigned int) pixel->val[0]) {

                    disp.x += j;
                    disp.y += i;
                    k++;

                }

            }

        }

        disp /= k;
        disp -= center;

    } else {

        std::cout << std::endl << "FRAME VAZIO" << std::endl;

    }

    cv::imshow("stupid", gray);

    return disp;

}
