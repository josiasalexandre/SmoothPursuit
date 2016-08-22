#ifndef STUPID_SLIP_H
#define STUPID_SLIP_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/tracking.hpp>

class StupidSlip {

    private:

        // the gray converted image
        cv::Mat gray;

        // the max value
        unsigned int threshold;

        // the orb detector and extractor
        cv::Ptr<cv::xfeatures2d::SurfFeatureDetector> detector;
        cv::Ptr<cv::xfeatures2d::SurfDescriptorExtractor> extractor;
        cv::Ptr<cv::ORB> orb;

        // the tld tracker
        cv::Ptr<cv::Tracker> tld;

        // the tld bounding box
        cv::Rect2d bounding;

        // the matcher
        cv::FlannBasedMatcher matcher;

        // the matches
        std::vector<cv::DMatch> matches;

        // show the matches
        cv::Mat img_matches;

        //the keypoints
        std::vector<cv::KeyPoint> keypoints_object, keypoints_scene;

        // the know object
        cv::Mat img_object;

        // the descriptors
        cv::Mat descriptors_object, descriptors_scene;

        // object position
        cv::Point2f position;

    public:

        // basic constructor
        StupidSlip(unsigned int);

        // initialize the TLD tracker
        bool initialize(cv::Mat frame, cv::Rect);

        // calculates the displacement from the fovea's center
        cv::Point2f displacement(cv::Point2i center, cv::Mat gray);
        cv::Point2f stupid_displacement(cv::Point2i center, cv::Mat gray);

        // calculates the displacement from the fovea's center using the TLD tracker algorithm
        cv::Rect2i tld_displacement(cv::Mat frame);

        // calculates the displacement from the fovea's center using the TLD tracker algorithm
        cv::Rect2i tld_displacement(cv::Mat frame, cv::Rect);


};

#endif
