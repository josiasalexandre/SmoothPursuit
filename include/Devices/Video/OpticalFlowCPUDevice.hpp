#ifndef OPTICAL_FLOW_DEVICE_H
#define OPTICAL_FLOW_DEVICE_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <SingleInputDevice.hpp>
#include <VideoSignalDevice.hpp>

class OpticalFlowCPUDevice : virtual public SingleInputDevice<cv::Mat, cv::Point2f> {

    private:

        cv::TermCriteria termcrit;
        cv::Size subPixWinSize, winSize;


        // the frame rate
        float dt;

        // an empty matrix
        cv::Mat empty_matrix;

        // just to avoid a lot of allocs
        cv::Mat image0, image1;

        bool first_time;

        std::vector<cv::Point2f> features_prev, features_next;

        std::vector<uchar> features;
        cv::Mat err;

    public:

        // basic constructor
        OpticalFlowCPUDevice() :
                                    dt(1000.0/30.0),
                                    first_time(false),
                                    termcrit(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 20, 0.03),
                                    subPixWinSize(10,10), winSize(31,31)
        {

            SingleInputDevice<cv::Mat, cv::Point2f>::input.first.set_null_value(empty_matrix);

        }

        // basic constructor
        OpticalFlowCPUDevice(cv::Mat v_null) :
                                                SingleInputDevice<cv::Mat, cv::Point2f>::SingleInputDevice(v_null),
                                                dt(1000.0/30.0),
                                                first_time(false),
                                                termcrit(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 20, 0.03),
                                                subPixWinSize(10,10), winSize(31,31) {}

        // basic destructor

        // @overriding the connect method
        // connect two devices
        virtual void connect(BaseDevice *dev) {

            if (nullptr != dev && this != dev) {

                // try to dowcast to DeviceOutput pointer
                DeviceOutput<cv::Mat> *out = dynamic_cast<DeviceOutput<cv::Mat> *>(dev);

                // try to dowcast to DeviceInput pointer
                DeviceInput<cv::Point2f> *in = dynamic_cast<DeviceInput<cv::Point2f> *>(dev);

                if (nullptr != in) {

                    // connect the current device's output to the external device's input
                    in->add_signal_source(this);

                } else if (nullptr != out) {

                    // connec the external device's output to this current device's input
                    // reverse case
                    add_signal_source(dev);

                    // try to update the frame rate
                    VideoSignalDevice *vsd = dynamic_cast<VideoSignalDevice *>(dev);

                    if(nullptr != vsd) {

                        dt = (float) 1000.0/(vsd->get_fps());

                    }

                }

            }

        }

        // @overriding the main method
        virtual void run() {

            if (1 < SingleInputDevice<cv::Mat, cv::Point2f>::input.first.get_size()) {

                // get the images from the the input buffer
                cv::Mat frame0 = SingleInputDevice<cv::Mat, cv::Point2f>::input.first.pop().clone();
                cv::Mat frame1 = SingleInputDevice<cv::Mat, cv::Point2f>::input.first.get_current().clone();

                // the mean
                cv::Point2f mean(0.0, 0.0);

                if(!frame0.empty() && !frame1.empty()) {

                    // convert to grayscale
                    cv::cvtColor(frame0, image0, cv::COLOR_BGR2GRAY);

                    // convert to grayscale
                    cv::cvtColor(frame1, image1, cv::COLOR_BGR2GRAY);

                    if(!first_time) {

                        cv::goodFeaturesToTrack(image1, features_next, 500, 0.01, 10, cv::Mat(), 3, 0, 0.4);
                        cv::cornerSubPix(image1, features_next, subPixWinSize, cv::Size(-1,-1), termcrit);

                        first_time = true;
                        std::cout << std::endl << "Primeira vez" << std::endl;

                    }

                    features_prev = features_next;

                    cv::calcOpticalFlowPyrLK(image0, image1, features_prev, features_next, features, err, winSize, 3, termcrit, 0, 0.001);

                    int k;
                    for (int i = k = 0; i < features_next.size(); i++) {

                        cv::line(frame1, features_prev[i], features_next[i], cv::Scalar(0,255,0), 2);
                        if (features[i]) {
                            features_next[k++] = features_next[i];

                        }

                    }

                    features_next.resize(k);

                    cv::imshow("frame", frame1);

                    cv::waitKey(100);

                    // push to outputs
                    DeviceOutput<cv::Point2f>::send(mean);

                } else {

                    // push to outputs
                    DeviceOutput<cv::Point2f>::send(mean);

                }

//                 std::cout << std::endl << "Mean (" << mean.x << ", " << mean.y << ")" << std::endl;

            }


        }

};

#endif
