#include <StupidSlip.hpp>
#include <iostream>

// basic constructor
StupidSlip::StupidSlip(unsigned int value) : threshold(value), bounding() {

     //load the image
    img_object = cv::imread("../Examples/Lemming/Lemming_reference.png", CV_LOAD_IMAGE_GRAYSCALE);

    // verify image
    if (img_object.empty()) {
        throw std::invalid_argument("Could not open the reference image!");
    }

    // build the detector and extractor
    detector = cv::xfeatures2d::SurfFeatureDetector::create(600);
    extractor = cv::xfeatures2d::SURF::create();

    // detect keypoints at the reference image
    //orb->detect(img_object, keypoints_object);
    detector->detect(img_object, keypoints_object);

    // compute the descriptors
    //orb->compute(img_object, keypoints_object, descriptors_object);
    extractor->compute(img_object, keypoints_object, descriptors_object);

}

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

cv::Point2f StupidSlip::displacement(cv::Point2i center, cv::Mat gray) {

    // detect the keypoints at the fovea
    detector->detect(gray, keypoints_scene);

    // find the object
    //orb->compute(gray, keypoints_scene, descriptors_scene);
    extractor->compute(gray, keypoints_scene, descriptors_scene);

    // clear the vector
    matches.clear();

    // find the matches
    matcher.match(descriptors_object, descriptors_scene, matches);

    // reset the position
    position.x = 0.0;
    position.y = 0.0;

    if (0 < matches.size()) {

        for (int i = 0; i < matches.size(); i++) {

            position += keypoints_scene[matches[i].trainIdx].pt;

        }

        // average
        position.x = (position.x/matches.size()) - center.x;
        position.y = (position.y/matches.size()) - center.y;

        // draw the matches
        if (0 < keypoints_scene.size()) {

            cv::drawMatches(img_object, keypoints_object, gray, keypoints_scene, matches, img_matches);

        }

    }

    // show the image
    cv::imshow("matches", img_matches);

    return position;

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
