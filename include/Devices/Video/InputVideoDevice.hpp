#ifndef INPUT_VIDEO_DEVICE_H
#define INPUT_VIDEO_DEVICE_H

#include <QWidget>

#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <InputSignalDevice.hpp>

class InputVideoDevice : public virtual InputSignalDevice<cv::Mat> {

    private:

        // the current output buffer refference
        DeviceInputBufferRef<cv::Mat> output;

        // the video
        cv::VideoCapture video;

        // a frame
        cv:: Mat frame;

        // the update rate
        unsigned int rate;


    public:

        // basic constructor - camera id
        InputVideoDevice(int id) : video(id), rate(30) {

            if (!video.isOpened()) {

                // throw an exception
                throw std::invalid_argument("invalid device id");

            }

        }

        // basic constructor, now with fps
        InputVideoDevice(int id, float fps) : video(id), rate(1000/fps) {

            if (!video.isOpened()) {

                // throw an exception
                throw std::invalid_argument("invalid device id");

            }

        }

        // basic constructor- filename
        InputVideoDevice(const std::string filename, float fps) : video(filename), rate(1000/fps) {

            if (!video.isOpened()) {

                // throw an exception
                throw std::invalid_argument("unable to open the file");

            }

        }

        // basid destructor
        ~InputVideoDevice () {

            if (video.isOpened()) {

                video.release();

            }

        }

        // the main method
        virtual void run() {

            // get the first frame
            video >> frame;

            // the keyboard char
            int key = 0;

            // the named window
            cv::namedWindow("frame", cv::WINDOW_AUTOSIZE);

            while(!frame.empty() && (char) key != 'q') {

                // draw the image
                cv::imshow("frame", frame);

                // send to external devices

                // wait for the input
                key = cv::waitKey(rate);

                // get next frame
                video >> frame;

            }

        }


            // connect two devices
        virtual void connect(BaseDevice *) {};

        // disconnect two devices
        virtual void disconnect(BaseDevice *) {};
};

#endif
