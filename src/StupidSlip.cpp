#include <StupidSlip.hpp>
#include <iostream>

// basic constructor
StupidSlip::StupidSlip(unsigned int value) : threshold(value), bounding() {}

// initialize the TLD tracker
bool StupidSlip::initialize(cv::Mat frame, cv::Rect fovea) {

    // the tld version
    tld = cv::Tracker::create("BOOSTING");

    // create a new tracker
    bounding = fovea;

    if (tld->init(frame, bounding)) {

        cv::rectangle(frame, bounding, cv::Scalar( 255, 0, 0 ), 2, 1);

        return true;

    }

    return false;

}

cv::Point2f StupidSlip::stupid_displacement(cv::Point2i center, cv::Mat frame) {

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

cv::Rect2i StupidSlip::tld_displacement(cv::Mat frame) {

    // update the tld
    tld->update(frame, bounding);

    cv::rectangle(frame, bounding, cv::Scalar( 255, 0, 0 ), 2, 1);

    return bounding;

}

cv::Rect2i StupidSlip::tld_displacement(cv::Mat frame, cv::Rect external_frame) {

    // update the tld
    tld->update(frame(external_frame).clone(), bounding);

    // move the bounding box to the external frame reference
    cv::Rect internal_frame(external_frame.x + bounding.x, external_frame.y + bounding.y, bounding.width, bounding.height);

    cv::rectangle(frame, internal_frame, cv::Scalar( 255, 0, 0 ), 2, 1);

    return internal_frame;

}
