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

        // the frame fps
        float fps, dt;

        // just to avoid a lot of allocs
        cv::Mat frame, flow, cflow;

        //
        cv::UMat gray, old_gray, uflow;

    public:

        // basic constructor
        OpticalFlowCPUDevice(cv::Mat v_null) :
                                                SingleInputDevice<cv::Mat, cv::Point2f>::SingleInputDevice(v_null),
                                                dt(1.0/30) {}

        // basic destructor
        virtual ~OpticalFlowCPUDevice() {}

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

                    // try to update the fps
                    VideoSignalDevice *vsd = dynamic_cast<VideoSignalDevice *>(dev);

                    if(nullptr != vsd) {

                        fps = vsd->get_fps();

                        dt = 1.0/fps;

                    }

                }

            }

        }

        // @overriding the main method
        virtual void run() {

            // the mean
            cv::Point2f mean(0.0, 0.0);

            CircularBuffer<cv::Mat> *buffer = SingleInputDevice<cv::Mat, cv::Point2f>::get_buffer();

            if (0 < buffer->get_size()) {


                // get the images from the the input buffer
                frame = buffer->pop();

                // convert to gray scale
                cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

                if(!old_gray.empty()) {

                    cv::calcOpticalFlowFarneback(old_gray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);

                    uflow.copyTo(flow);

                    for(int i = 0; i < flow.rows; i++) {

                        cv::Point2f *row = flow.ptr<cv::Point2f>(i);

                        for(int j = 0; j < flow.cols; j++) {

                            mean += row[j];

                        }

                    }

                }

                std::swap(old_gray, gray);

                if (0 != flow.rows && 0 != flow.cols) {

                    mean /= (((float) flow.rows*flow.cols)*dt);

                } else {

                    mean.x = 0.0;
                    mean.y = 0.0;

                }

            }

            // push to outputs
            DeviceOutput<cv::Point2f>::send(mean);

        }

        // draw the optical flow
        virtual void drawOpticalFlow(cv::Mat &flow) {

            cv::Mat xy[2]; //X,Y
            cv::split(flow, xy);

            //calculate angle and magnitude
            cv::Mat magnitude, angle;
            cv::cartToPolar(xy[0], xy[1], magnitude, angle, true);

            //translate magnitude to range [0;1]
            double mag_max;
            cv::minMaxLoc(magnitude, nullptr, &mag_max);
            magnitude.convertTo(magnitude, -1, 1.0/mag_max);

            //build hsv image
            cv::Mat _hls[3], hsv;
            _hls[0] = angle;
            _hls[1] = cv::Mat::ones(angle.size(), CV_32F);
            _hls[2] = magnitude;
            cv::merge(_hls, 3, hsv);

            //convert to BGR and show
            cv::Mat bgr;
            cv::cvtColor(hsv, bgr, cv::COLOR_HSV2BGR);

            cv::imshow("frame", frame);
            cv::imshow("optical flow", bgr);

            cv::waitKey(15);

        }

        // add a signal source to the current input
        virtual void add_signal_source(BaseDevice *dev) {

            SingleInputDevice<cv::Mat, cv::Point2f>::add_signal_source(dev);

            // try to update the fps
            VideoSignalDevice *vsd = dynamic_cast<VideoSignalDevice *>(dev);

            if(nullptr != vsd) {

                fps = vsd->get_fps();

                dt = 1.0/fps;

            }

        }

                // disconnect the a signal source from the current inputs
        virtual void disconnect_signal_source(BaseDevice *dev) {

            SingleInputDevice<cv::Mat, cv::Point2f>::disconnect_signal_source(dev);

        }
};

#endif
