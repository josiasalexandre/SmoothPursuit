#ifndef INPUT_VIDEO_DEVICE_H
#define INPUT_VIDEO_DEVICE_H

#include <unistd.h>

#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>

#include <InputSignalDevice.hpp>
#include <VideoSignalDevice.hpp>

class InputVideoDevice : public virtual InputSignalDevice<cv::Mat>, public virtual VideoSignalDevice {

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
        InputVideoDevice(int id) : video(id) {

            if (!video.isOpened()) {

                // throw an exception
                throw std::invalid_argument("invalid device id");

            }

            rate = 1e6/30.0;

        }

        // basic constructor, now with fps
        InputVideoDevice(int id, float fps) : video(id) {

            if (!video.isOpened()) {

                // throw an exception
                throw std::invalid_argument("invalid device id");

            }

            if (0.0 == fps) {

                fps = 30;

            }

            rate = 1e6/fps;

        }

        // basic constructor- filename
        InputVideoDevice(const std::string filename, float fps) : video(filename) {

            if (!video.isOpened()) {

                // throw an exception
                throw std::invalid_argument("unable to open the file");

            }

            if (0.0 == fps) {

                fps = 30;

            }

            rate = 1e6/fps;

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

            if(!frame.empty()) {

                // lock the output vector
                DeviceOutput<cv::Mat>::output_mutex.lock();

                // send the current image to the output
                unsigned int out_size = DeviceOutput<cv::Mat>::output.size();

                if (0 < out_size) {

                    for (int i = 0; i < out_size; i++) {

                        // send the image frames
                        DeviceOutput<cv::Mat>::output[i].first->push(frame);

                    }

                }

                // based on the fps
                // usleep(rate);

                // unlock the output vector
                DeviceOutput<cv::Mat>::output_mutex.unlock();

            }

        }

        // get the frame rate
        virtual float get_fps() {

            return (float) 1e6/((float) rate);

        }

};

#endif
